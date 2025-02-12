#include "AAIPawn.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "LuaManger.h"
#include "NavigationSystem.h"
#include "AbilitySystem/CustomAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/CombatAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

FName AAIPawn::AbilitySystemComponentName(TEXT("AbilitySystemComponent"));

AAIPawn::AAIPawn(const FObjectInitializer& ObjectInitializer)
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->InitCapsuleSize(34.0f, 88.0f);
	Capsule->CanCharacterStepUpOn = ECB_No;
	Capsule->SetShouldUpdatePhysicsVolume(false);
	Capsule->SetCanEverAffectNavigation(false);
	Capsule->bDynamicObstacle = true;
	RootComponent = Capsule;

	AbilitySystemComponent = CreateDefaultSubobject<UCustomAbilitySystemComponent>(AbilitySystemComponentName);
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->UpdatedComponent = Capsule;

	USkeletalMeshComponent* MeshComponent = GetMesh();
	MeshComponent->AlwaysLoadOnClient = true;
	MeshComponent->AlwaysLoadOnServer = true;
	MeshComponent->bOwnerNoSee = false;
	MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	MeshComponent->bCastDynamicShadow = true;
	MeshComponent->bAffectDynamicIndirectLighting = true;
	MeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	MeshComponent->SetupAttachment(Capsule);
	static FName MeshCollisionProfileName(TEXT("Pawn"));
	MeshComponent->SetCollisionProfileName(MeshCollisionProfileName);
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetCanEverAffectNavigation(false);

	CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet"));
	AbilitySystemComponent->AddAttributeSetSubobject(CombatAttributeSet);
	CombatAttributeSet->OnOutOfHealth.AddUObject(this, &AAIPawn::OnDead);

	bLockRotationToActor = false;
}

void AAIPawn::OnDead(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec,
                     float DamageMagnitude, AActor* AvatarActor)
{
	Die(DamageInstigator, DamageCauser, DamageEffectSpec, DamageMagnitude, AvatarActor);
}

void AAIPawn::RotateToTargetRotation(const FRotator& TargetRotation, float UsedRotateSpeed)
{
	delete TargetRotator;
	TargetRotator = new FRotator(TargetRotation);
	CurrentRotateSpeed = UsedRotateSpeed == -1 ? RotateSpeed : UsedRotateSpeed;
}

void AAIPawn::OnRep_IsDead()
{
}

void AAIPawn::SetUpProperties()
{
	if (AAIController* AIController = Cast<AAIController>(Controller))
	{
		MoveToTaskPtr = UAITask::NewAITask<UAITask_MoveTo>(*AIController, EAITaskPriority::High);
	}
}

void AAIPawn::FindRandomLocation(const FVector& CenterLocation, FVector& ResultLocation, const float Range)
{
	if (GetCharacterMovement()->DefaultLandMovementMode == EMovementMode::MOVE_Walking)
	{
		UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, CenterLocation, ResultLocation, Range);
	}
	else if (GetCharacterMovement()->DefaultLandMovementMode == EMovementMode::MOVE_Flying)
	{
		const float RandomAngle = 2.f * PI * FMath::FRand();
		const float U = FMath::FRand() + FMath::FRand();
		const float RandomRadius = Range * (U > 1 ? 2.f - U : U);
		const FVector RandomOffset(FMath::Cos(RandomAngle) * RandomRadius, FMath::Sin(RandomAngle) * RandomRadius, 0);
		ResultLocation = CenterLocation + RandomOffset;
	}
}

void AAIPawn::BeginPlay()
{
	if (!IsNetMode(ENetMode::NM_DedicatedServer))
	{
		USkeletalMeshComponent* MeshComponent = GetMesh();
		MeshComponent->bEnableUpdateRateOptimizations = true;
		/*MeshComponent->AnimUpdateRateParams->MaxEvalRateForInterpolation = 5;
		MeshComponent->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Reset();
		MeshComponent->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Emplace(0.01);
		MeshComponent->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Emplace(0.005);
		MeshComponent->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Emplace(0.003);
		MeshComponent->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Emplace(0.0005);*/
		MeshComponent->AnimUpdateRateParams->BaseNonRenderedUpdateRate = 8;
		const TArray<FSignificanceSettings::FSignificanceThresholds> Thresholds{
			{3, 3000}, {2, 4000}, {1, 5000}, {0, 20000}, {-1, 50000}
		};
		SetSignificanceSettings(Thresholds);
		SignificanceRegisterObject(this,TEXT("Monster"), GetWorld());
	}
	AddAbilities();
	Super::BeginPlay();
}

void AAIPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	SignificanceUnregisterObject(this, GetWorld());
}

void AAIPawn::FlyTo(FVector InGoalLocation, AActor* InGoalActor, float AcceptanceRadius, EAIOptionFlag::Type StopOnOverlap, EAIOptionFlag::Type AcceptPartialPath, bool bUsePathfinding, bool bLockAILogic, bool bUseContinuousGoalTracking, EAIOptionFlag::Type ProjectGoalOnNavigation)
{
	if (AAIController* AIController = Cast<AAIController>(Controller))
	{
		if (FlyToTaskPtr == nullptr)
		{
			FlyToTaskPtr = UAITask::NewAITask<UAITask_FlyTo>(*AIController, EAITaskPriority::High);
		}
		if (FlyToTaskPtr)
		{
			FAIMoveRequest MoveReq;
			if (InGoalActor)
			{
				MoveReq.SetGoalActor(InGoalActor);
			}
			else
			{
				MoveReq.SetGoalLocation(InGoalLocation);
			}

			MoveReq.SetAcceptanceRadius(AcceptanceRadius);
			MoveReq.SetReachTestIncludesAgentRadius(FAISystem::PickAIOption(StopOnOverlap, MoveReq.IsReachTestIncludingAgentRadius()));
			MoveReq.SetAllowPartialPath(FAISystem::PickAIOption(AcceptPartialPath, MoveReq.IsUsingPartialPaths()));
			MoveReq.SetUsePathfinding(bUsePathfinding);
			MoveReq.SetProjectGoalLocation(FAISystem::PickAIOption(ProjectGoalOnNavigation, MoveReq.IsProjectingGoal()));
			if (Controller)
			{
				MoveReq.SetNavigationFilter(AIController->GetDefaultNavigationFilterClass());
			}

			FlyToTaskPtr->SetUp(AIController, MoveReq);
			FlyToTaskPtr->SetContinuousGoalTracking(bUseContinuousGoalTracking);

			if (bLockAILogic)
			{
				FlyToTaskPtr->RequestAILogicLocking();
			}
		}
	}
}

void AAIPawn::MoveTo(FVector InGoalLocation, AActor* InGoalActor, float AcceptanceRadius,
                     EAIOptionFlag::Type StopOnOverlap, EAIOptionFlag::Type AcceptPartialPath, bool bUsePathfinding,
                     bool bLockAILogic,
                     bool bUseContinuousGoalTracking, EAIOptionFlag::Type ProjectGoalOnNavigation)
{
	if (AAIController* AIController = Cast<AAIController>(Controller))
	{
		if (MoveToTaskPtr == nullptr)
		{
			MoveToTaskPtr = UAITask::NewAITask<UAITask_MoveTo>(*AIController, EAITaskPriority::High);
		}
		if (MoveToTaskPtr)
		{
			FAIMoveRequest MoveReq;
			if (InGoalActor)
			{
				MoveReq.SetGoalActor(InGoalActor);
			}
			else
			{
				MoveReq.SetGoalLocation(InGoalLocation);
			}

			MoveReq.SetAcceptanceRadius(AcceptanceRadius);
			MoveReq.SetReachTestIncludesAgentRadius(FAISystem::PickAIOption(StopOnOverlap, MoveReq.IsReachTestIncludingAgentRadius()));
			MoveReq.SetAllowPartialPath(FAISystem::PickAIOption(AcceptPartialPath, MoveReq.IsUsingPartialPaths()));
			MoveReq.SetUsePathfinding(bUsePathfinding);
			MoveReq.SetProjectGoalLocation(FAISystem::PickAIOption(ProjectGoalOnNavigation, MoveReq.IsProjectingGoal()));
			if (Controller)
			{
				MoveReq.SetNavigationFilter(AIController->GetDefaultNavigationFilterClass());
			}

			MoveToTaskPtr->SetUp(AIController, MoveReq);
			MoveToTaskPtr->SetContinuousGoalTracking(bUseContinuousGoalTracking);

			if (bLockAILogic)
			{
				MoveToTaskPtr->RequestAILogicLocking();
			}
		}
	}
}

void AAIPawn::OnNearWeaponHit(const FHitResult& HitResult, const FName& HitSocketName) const
{
}

UAbilitySystemComponent* AAIPawn::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAIPawn::AddAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || AbilitySystemComponent->bAbilitiesGiven)
	{
		return;
	}
	K2_AddAbilities();
	AbilitySystemComponent->bAbilitiesGiven = true;
}

FGameplayAbilitySpecHandle AAIPawn::K2_GiveAbility(TSubclassOf<UCustomGameplayAbility> Ability, int32 Lv)
{
	return AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, Lv, INDEX_NONE, this));
	// return *new FGameplayAbilitySpecHandle();
}

float AAIPawn::GetAttributeValueByName(FName AttributeName)
{
	if (!CombatAttributeSet)
	{
		return 0.f;
	}
	const FGameplayAttribute Attribute = UCustomAttributeSet::GetPropertyByName(UCombatAttributeSet::StaticClass(), AttributeName);
	return Attribute.GetNumericValue(CombatAttributeSet);
}

USkeletalMeshComponent* AAIPawn::GetRightWeaponMesh() const
{
	return GetRightHandHoldWeaponMesh();
}

USkeletalMeshComponent* AAIPawn::GetLeftWeaponMesh() const
{
	return GetLeftHandHoldWeaponMesh();
}


UAITask_MoveTo* AAIPawn::MoveToLocation(FVector Position, float AcceptanceRadius, EAIOptionFlag::Type StopOnOverlap,
                                        EAIOptionFlag::Type AcceptPartialPath,
                                        bool bUsePathfinding, bool bLockAILogic, bool bUseContinuosGoalTracking,
                                        EAIOptionFlag::Type ProjectGoalOnNavigation)
{
	if (GetCharacterMovement()->DefaultLandMovementMode == EMovementMode::MOVE_Walking)
	{
		MoveTo(Position, nullptr, AcceptanceRadius, StopOnOverlap, AcceptPartialPath, bUsePathfinding, bLockAILogic,
		       bUseContinuosGoalTracking, ProjectGoalOnNavigation);
		return MoveToTaskPtr;
	}
	else if (GetCharacterMovement()->DefaultLandMovementMode == EMovementMode::MOVE_Flying)
	{
		FlyTo(Position, nullptr, AcceptanceRadius, StopOnOverlap, AcceptPartialPath, bUsePathfinding, bLockAILogic,
		      bUseContinuosGoalTracking, ProjectGoalOnNavigation);
		return FlyToTaskPtr;
	}
	else
	{
		return nullptr;
	}
}

UAITask_MoveTo* AAIPawn::MoveToActor(AActor* ActorPtr, float AcceptanceRadius, EAIOptionFlag::Type StopOnOverlap,
                                     EAIOptionFlag::Type AcceptPartialPath,
                                     bool bUsePathfinding, bool bLockAILogic, bool bUseContinuosGoalTracking,
                                     EAIOptionFlag::Type ProjectGoalOnNavigation)
{
	if (IsValid(ActorPtr))
	{
		if (GetCharacterMovement()->DefaultLandMovementMode == EMovementMode::MOVE_Walking)
		{
			MoveTo(ActorPtr->GetActorLocation(), ActorPtr, AcceptanceRadius, StopOnOverlap, AcceptPartialPath,
			       bUsePathfinding, bLockAILogic, bUseContinuosGoalTracking, ProjectGoalOnNavigation);
			return MoveToTaskPtr;
		}
		else if (GetCharacterMovement()->DefaultLandMovementMode == EMovementMode::MOVE_Flying)
		{
			FlyTo(ActorPtr->GetActorLocation(), ActorPtr, AcceptanceRadius, StopOnOverlap, AcceptPartialPath,
			      bUsePathfinding, bLockAILogic, bUseContinuosGoalTracking, ProjectGoalOnNavigation);
			return FlyToTaskPtr;
		}
		else
		{
			return nullptr;
		}
	}
	return nullptr;
}

void AAIPawn::LockLookAtPos(const FVector& TargetLocation, float UsedRotateSpeed)
{
	bLockRotationToLocation = true;
	bLockRotationToActor = false;
	LockedLocation = TargetLocation;
	CurrentRotateSpeed = UsedRotateSpeed == -1 ? RotateSpeed : UsedRotateSpeed;
}

void AAIPawn::LockLookAtActor(AActor* Actor, float UsedRotateSpeed)
{
	bLockRotationToActor = true;
	bLockRotationToLocation = false;
	LockedActor = Actor;
	CurrentRotateSpeed = UsedRotateSpeed == -1 ? RotateSpeed : UsedRotateSpeed;
}

void AAIPawn::UnlockLookAt()
{
	bLockRotationToLocation = false;
	bLockRotationToActor = false;
	LockedActor = nullptr;
}

void AAIPawn::SetSpeed(float Speed)
{
	if (GetCharacterMovement()->DefaultLandMovementMode == EMovementMode::MOVE_Walking)
	{
		GetCharacterMovement()->MaxWalkSpeed = Speed;
	}
	else if (GetCharacterMovement()->DefaultLandMovementMode == EMovementMode::MOVE_Flying)
	{
		GetCharacterMovement()->MaxFlySpeed = Speed;
	}
}

float AAIPawn::GetSpeed()
{
	if (GetCharacterMovement()->DefaultLandMovementMode == EMovementMode::MOVE_Walking)
	{
		return GetCharacterMovement()->MaxWalkSpeed;
	}
	else if (GetCharacterMovement()->DefaultLandMovementMode == EMovementMode::MOVE_Flying)
	{
		return GetCharacterMovement()->MaxFlySpeed;
	}
	return 0;
}

void AAIPawn::StopMovementImmediately(const bool bClearDelegate)
{
	if (bClearDelegate && IsValid(FlyToTaskPtr))
	{
		FlyToTaskPtr->OnRequestFailed.Clear();
		FlyToTaskPtr->OnMoveFinished.Clear();
	}
	if (bClearDelegate && IsValid(MoveToTaskPtr))
	{
		MoveToTaskPtr->OnRequestFailed.Clear();
		MoveToTaskPtr->OnMoveFinished.Clear();
	}
	GetCharacterMovement()->StopMovementImmediately();
}

void AAIPawn::SetRotationFollowVelocity(bool bFollow)
{
	bRotationFollowVelocity = bFollow;
}

void AAIPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (IsRunningDedicatedServer())
	{
		const FRotator CurrentRotator = GetActorRotation();
		if (bLockRotationToActor)
		{
			if (LockedActor.IsValid())
			{
				delete TargetRotator;
				TargetRotator = new FRotator(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockedActor.Get()->GetActorLocation()));
				TargetRotator->Pitch = CurrentRotator.Pitch;
				TargetRotator->Roll = CurrentRotator.Roll;
			}
		}
		else if (bLockRotationToLocation)
		{
			delete TargetRotator;
			TargetRotator = new FRotator(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockedLocation));
			TargetRotator->Pitch = CurrentRotator.Pitch;
			TargetRotator->Roll = CurrentRotator.Roll;
		}
		else if (bRotationFollowVelocity && GetVelocity().Size2D() > 10 && !GetMesh()->IsPlayingRootMotion())
		{
			delete TargetRotator;
			TargetRotator = new FRotator(UKismetMathLibrary::FindLookAtRotation(FVector(0, 0, 0), GetVelocity()));
			TargetRotator->Pitch = CurrentRotator.Pitch;
			TargetRotator->Roll = CurrentRotator.Roll;
			CurrentRotateSpeed = RotateSpeed;
		}

		if (TargetRotator)
		{
			if (TargetRotator->Equals(CurrentRotator, DeltaSeconds * CurrentRotateSpeed))
			{
				SetActorRotation(*TargetRotator);
				delete TargetRotator;
				TargetRotator = NULL;
				CurrentRotateSpeed = RotateSpeed;
			}
			else
			{
				const FRotator Rotator = FMath::RInterpTo(CurrentRotator, *TargetRotator, DeltaSeconds, CurrentRotateSpeed / (DeltaSeconds * 360));
				SetActorRotation(Rotator);
			}
		}
	}
}

void AAIPawn::UpdateSignificanceSettings(const TMap<float, float>& SignificanceThresholdsMap)
{
	TArray<FSignificanceSettings::FSignificanceThresholds> Thresholds;
	for (auto Threshold : SignificanceThresholdsMap)
	{
		Thresholds.Emplace(Threshold.Key, Threshold.Value);
	}
	SetSignificanceSettings(Thresholds);
}


float AAIPawn::SignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo,
                                    const FTransform& Viewpoint)
{
	if (ObjectInfo->GetTag() == TEXT("Monster"))
	{
		const AAIPawn* Pawn = CastChecked<AAIPawn>(ObjectInfo->GetObject());
		if (Pawn)
		{
			const APlayerController* c = GetWorld()->GetFirstPlayerController();
			const float fovScale = c->PlayerCameraManager->GetFOVAngle() / c->PlayerCameraManager->DefaultFOV;
			float Distance = (Pawn->GetActorLocation() - Viewpoint.GetLocation()).Size();
			Distance *= fovScale;
			float Significance = GetSignificanceByDistance(Distance);
			if (Significance < 3)
			{
				const USkeletalMeshComponent* MeshComponent = GetMesh();
				if (MeshComponent && !MeshComponent->WasRecentlyRendered(0.1f))
					if (MeshComponent)
					{
						Significance = -1;
					}
			}
			return Significance;
		}
	}
	return 0;
}

void AAIPawn::PostSignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo,
                                       float OldSignificance, float Significance, bool bFinal)
{
	if (ObjectInfo->GetTag() == TEXT("Monster"))
	{
		const AAIPawn* Pawn = CastChecked<AAIPawn>(ObjectInfo->GetObject());
		if (Pawn)
		{
			USkeletalMeshComponent* MeshComponent = GetMesh();
			UCapsuleComponent* Capsule = GetCapsuleComponent();
			MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
			Capsule->SetGenerateOverlapEvents(false);
			if (Significance == -1)
			{
				SetTickInterval(0.1);
			}
			else if (Significance == 0)
			{
				SetTickInterval(1 / 20);
				MeshComponent->SetComponentTickInterval(1 / 20);
			}
			else if (Significance == 1)
			{
				SetTickInterval(1 / 30);
				MeshComponent->SetComponentTickInterval(1 / 30);
			}
			else if (Significance == 2)
			{
				SetTickInterval(1 / 45);
				MeshComponent->SetComponentTickInterval(1 / 30);
			}
			else if (Significance == 3)
			{
				SetTickInterval(1 / 60);
				MeshComponent->SetComponentTickInterval(1 / 30);
				Capsule->SetGenerateOverlapEvents(true);
			}
		}
	}
}

void AAIPawn::SetTickInterval(float Interval)
{
	SetActorTickInterval(Interval);
	for (const auto Component : GetComponents())
	{
		if (Component)
		{
			Component->SetComponentTickInterval(Interval);
		}
	}
}

float AAIPawn::GetHealth()
{
	if (!CombatAttributeSet)
	{
		return 0.f;
	}

	return CombatAttributeSet->GetHealth();
}

float AAIPawn::GetMaxHealth()
{
	if (!CombatAttributeSet)
	{
		return 0.f;
	}

	return CombatAttributeSet->GetMaxHealth();
}

void AAIPawn::InitAttributesFromLua(TMap<FString, float> InAttributes)
{
	if (CombatAttributeSet)
	{
		for (auto It : InAttributes)
		{
			FGameplayAttributeData* ValueField = nullptr;
			if (FStructProperty* TargetField = FindFProperty<FStructProperty>(UCombatAttributeSet::StaticClass(), *It.Key))
			{
				ValueField = TargetField->ContainerPtrToValuePtr<FGameplayAttributeData>(CombatAttributeSet);
			}

			if (ValueField)
			{
				ValueField->SetBaseValue(It.Value);
				ValueField->SetCurrentValue(It.Value);
			}
			else
			{
				UE_LOG(GAME, Warning, TEXT("[AAIPawn]InitAttributesFromLua : Not find field named %s."), *It.Key)
			}
		}
	}
}

void AAIPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AAIPawn, IsDead, COND_None, REPNOTIFY_Always);
}
