#include "CharacterCheatChecker.h"

#include "Gameplay/SyncMoveComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"


UCharacterCheatChecker::UCharacterCheatChecker()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterCheatChecker::BeginPlay()
{
	Super::BeginPlay();
	CapsuleComponent = Cast<UCapsuleComponent>(GetOwner()->GetRootComponent());
}

void UCharacterCheatChecker::SelfPullBacked()
{
	UActorComponent* com = GetOwner()->GetComponentByClass(UCharacterMovementComponent::StaticClass());
	if (com)
	{
		UCharacterMovementComponent* moveCom=Cast<UCharacterMovementComponent>(com);
		moveCom->StopMovementImmediately();
	}
}

bool UCharacterCheatChecker::IsCollisionRadiusChange()
{
	float curRadius = CapsuleComponent->GetScaledCapsuleRadius();
	if (LastCollisionRadius != curRadius)
	{
		LastCollisionRadius = curRadius;
		return true;
	}
	return false;
}

bool UCharacterCheatChecker::IsTargetPosFly(FVector targetPos, EStateType state)
{
	if (state == EStateType::InAir || state == EStateType::Swim || state == EStateType::Climb || state ==
		EStateType::Ladder || state == EStateType::BackTo)
	{
		return false;
	}
	bool hasGround = false;
	FVector groundPos = GetGroundPos(targetPos, hasGround);
	if (hasGround && (targetPos.Z - groundPos.Z) < 200)
	{
		return false;
	}
	UE_LOG(LogTemp, Warning, TEXT("TargetPosFly,CurState:%d,  targetPos:  %s  groundPos:%s "), state,
	       *targetPos.ToString(), *groundPos.ToString())
	return true;
}

bool UCharacterCheatChecker::IsClimbPosOk(FVector targetPos, EStateType state)
{
	if (state == EStateType::Climb)
	{
		FHitResult OutHit;

		const bool bHit = UKismetSystemLibrary::SphereTraceSingle(this, targetPos + FVector::UpVector * 200,
		                                                          targetPos + FVector::DownVector * 100,
		                                                          100,
		                                                          UEngineTypes::ConvertToTraceType(
			                                                          ECC_GameTraceChannel1),
		                                                          false, IgnoreActors
		                                                          , EDrawDebugTrace::None, OutHit
		                                                          , true);
		if (!bHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("ClimbPos Error,TargetPos:  %s "),*targetPos.ToString())
			return false;
		}
	}
	return true;
}

bool UCharacterCheatChecker::IsLadderPosOk(FVector targetPos, EStateType state)
{
	if (state == EStateType::Ladder)
	{
		TArray<FHitResult> OutHits;

		const bool bHit = UKismetSystemLibrary::CapsuleTraceMulti(this, targetPos,
		                                                          targetPos + FVector::DownVector*10,
		                                                          100,110,
		                                                          UEngineTypes::ConvertToTraceType(
			                                                          ECC_GameTraceChannel10),
		                                                          false, IgnoreActors
		                                                          , EDrawDebugTrace::None, OutHits
		                                                          , true);
		const FName LadderBoxTag("ladderBox");
		for (auto OutHit : OutHits)
		{
			if (bHit && OutHit.GetComponent() && OutHit.GetComponent()->ComponentHasTag(LadderBoxTag))
			{
				return true;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("LadderPos Error,TargetPos:  %s "),*targetPos.ToString())
		return false;
	}
	return true;
}

bool UCharacterCheatChecker::IsCurvePosOk(FVector targetPos, EStateType state ,double timestamp)
{
	if (lastState==EStateType::BackTo && state == EStateType::BackTo)
	{
		  if (zMoveCurve != nullptr)
		  {
		  	float zOffset = zMoveCurve->GetFloatValue(timestamp - startCurveMoveTimeStamp);
		  	if (targetPos.Z - (zOffset + startCurveMovePos.Z) > 100)
		  	{
		  		return false;
		  	}
		  }
		  else
		  {
		  	UE_LOG(LogTemp, Warning, TEXT(" BackTo State MoveCurve Is Null"));
		  	return false;
		  }
	}
	return true;
}

bool UCharacterCheatChecker::PassSwimStateCheck(EStateType state)
{
	return Super::PassSwimStateCheck(state);
}

void UCharacterCheatChecker::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FVector UCharacterCheatChecker::GetGroundPos(FVector pos, bool& hasGround)
{
	FHitResult OutHit;
	const bool bHit = UKismetSystemLibrary::CapsuleTraceSingle(this, pos,
	                                                           pos + FVector::DownVector * 10000,
	                                                           CapsuleComponent->GetScaledCapsuleRadius(),
	                                                           CapsuleComponent->GetScaledCapsuleHalfHeight(),
	                                                           UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel9),
	                                                           false, IgnoreActors
	                                                           , EDrawDebugTrace::None, OutHit
	                                                           , true);
	if (bHit)
	{
		hasGround = true;
		return OutHit.ImpactPoint + FVector::UpVector * RootHeight;
	}
	hasGround = false;
	return CapsuleComponent->GetComponentLocation();
}
