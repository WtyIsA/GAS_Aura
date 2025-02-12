#include "Player/AttackTracer.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Gameplay/AttackTracerMgr.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/MyShootCharacter.h"

void UAttackTracer::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	if (IsServerPlayer())
	{
		return;
	}
	UAnimMontage* Montage = Cast<UAnimMontage>(Animation);
	/*FOnMontageEnded InOnMontageEnded = FOnMontageEnded::CreateLambda([this, MeshComp]()
   {
	   UAttackTracerMgr::Get()->AttackTracerInfoMap.Remove(MeshComp);
   });
	 MeshComp->GetAnimInstance()->Montage_SetEndDelegate(InOnMontageEnded, Montage);*/
	IWeaponOwnerInterface* Attacker = Cast<IWeaponOwnerInterface>(MeshComp->GetOwner());
	UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).Attacker = Attacker;
	if (Attacker)
	{
		UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).HitActors.Empty();
		UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).LastLocations.Empty();
		UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).ActorsToIgnore.Empty();
		USkeletalMeshComponent* Weapon = bRight ? Attacker->GetRightWeaponMesh() : Attacker->GetLeftWeaponMesh();
		UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).Weapon = Weapon;
		if (!Weapon)
		{
			return;
		}
		AActor* MeshOwner = MeshComp->GetOwner();
		UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).ActorsToIgnore.Add(MeshOwner);
		TArray<AActor*> OutActors;
		MeshOwner->GetAttachedActors(OutActors);
		for (auto OutActor : OutActors)
		{
			UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).ActorsToIgnore.Add(OutActor);
		}
		TArray<FName>& SocketNames = UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).SocketNames;
		SocketNames.Empty();
		for (auto kv : SocketName2HitBoxMap)
		{
			SocketNames.Add(kv.Key);
			UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).LastLocations.Add(
				Weapon->GetSocketLocation(kv.Key));
		}
	}
}

bool UAttackTracer::IsServerPlayer()
{
	return IsRunningDedicatedServer() && Owner->ActorHasTag("Player");
}

void UAttackTracer::ProcessHits(const TArray<FHitResult>& HitResultArray, const FName& HitSocketName,
                                USkeletalMeshComponent* MeshComp)
{
	for (const FHitResult& HitResult : HitResultArray)
	{
		AActor* HitActor = HitResult.GetActor();
		if (!UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).HitActors.Contains(HitActor) && !UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).HitActors.Contains(HitActor->GetOwner()))
		{
			CurHitResult = HitResult;
			CurHitSocketName = HitSocketName;
			UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).Attacker->OnNearWeaponHit(HitResult, HitSocketName);
			ProcessEventByName("Hited");
			OnHitted(HitResult, HitSocketName);
			TracerHittedEvent.Broadcast(MeshComp->GetOwner(), HitResult, HitSocketName);
			if (HitResult.GetActor()->IsA<APawn>())
			{
				UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).HitActors.Add(HitActor);
				if (CanStuck(HitResult,MeshComp))
				{
					BeginStuck(MeshComp);
				}
			}
			else
			{
				UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).HitActors.Add(HitActor->GetOwner());
				if (CanBounce(HitResult,MeshComp))
				{
					ProcessEventByName("Bound");
					BeginBounce(MeshComp);
					break;
				}
			}
		}
	}
}


bool UAttackTracer::CanStuck(const FHitResult& HitResult, USkeletalMeshComponent* MeshComp)
{
	return bOpenStuck && HitResult.GetActor()->IsA<APawn>();
}

bool UAttackTracer::CanBounce(const FHitResult& HitResult, USkeletalMeshComponent* MeshComp)
{
	return bOpenBound && !HitResult.GetActor()->ActorHasTag(TEXT("IgnoreBound")) && !UAttackTracerMgr::Get()->InStuck.
		Contains(MeshComp);
}

void UAttackTracer::BeginStuck(USkeletalMeshComponent* MeshComp)
{
	UAttackTracerMgr::Get()->BeginStuck(StuckTime, StuckPlayRate, MeshComp, CurAnimation);
}

void UAttackTracer::BeginBounce(USkeletalMeshComponent* MeshComp)
{
	UAttackTracerMgr::Get()->BeginBounce(BounceTime, BouncePlayRate, BounceBlendOutTime, MeshComp,
														 CurAnimation);
}

void UAttackTracer::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (IsServerPlayer())
	{
		return;
	}
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	USkeletalMeshComponent* Weapon = UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).Weapon;
	TArray<FName>& SocketNames = UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).SocketNames;
	if (!UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).Attacker || !Weapon || SocketNames.Num() == 0)
	{
		return;
	}
	TArray<FVector>& LastLocations = UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).LastLocations;
	TArray<FHitResult>& HitResults = UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).HitResults;
	if (Weapon)
	{
		for (int i = 0; i < SocketName2HitBoxMap.Num(); ++i)
		{
			if (i >= SocketNames.Num() || i >= LastLocations.Num())
			{
				UE_LOG(LogTemp, Warning, TEXT("SocketNames LastLocations error %d"), i);
				continue;
			}
			const FName SocketName = SocketNames[i];
			if (!SocketName2HitBoxMap.Contains(SocketName))
			{
				continue;
			}
			if (!TraceProfileName.IsNone())
			{
				UKismetSystemLibrary::BoxTraceMultiByProfile(Weapon->GetWorld(), LastLocations[i],
				                                             Weapon->GetSocketLocation(SocketName),
				                                             SocketName2HitBoxMap[SocketName],
				                                             Weapon->GetSocketRotation(SocketName),
				                                             TraceProfileName
				                                             , false,
				                                             UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).
				                                             ActorsToIgnore,
				                                             DrawDebugTrace, HitResults, true);
			}
			else
			{
				UKismetSystemLibrary::BoxTraceMulti(Weapon->GetWorld(), LastLocations[i],
				                                    Weapon->GetSocketLocation(SocketName),
				                                    SocketName2HitBoxMap[SocketName],
				                                    Weapon->GetSocketRotation(SocketName),
				                                    UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel6)
				                                    , false,
				                                    UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).
				                                                             ActorsToIgnore,
				                                    DrawDebugTrace, HitResults, true);
			}

			ProcessHits(HitResults, SocketName, MeshComp);
			LastLocations[i] = Weapon->GetSocketLocation(SocketName);
		}
	}
}

void UAttackTracer::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	UAttackTracerMgr::Get()->InStuck.Remove(MeshComp);
	if (IsServerPlayer())
	{
		return;
	}
	UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).HitActors.Empty();
	UAttackTracerMgr::Get()->GetAttackTracerInfo(MeshComp).LastLocations.Empty();
}
