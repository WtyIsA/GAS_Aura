// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ARider.h"

#include "Animation/AnimInstance.h"
#include "GameFramework/PlayerController.h"
#include "AIController.h"


// Sets default values
AARider::AARider()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AARider::BeginPlay()
{
	Super::BeginPlay();
	if (!IsNetMode(ENetMode::NM_DedicatedServer))
	{
		const TArray<FSignificanceSettings::FSignificanceThresholds> Thresholds{
			{3, 7000}, {2, 15000}, {1, 20000}, {0, 25000}, {-1, 50000}
		};
		SetSignificanceSettings(Thresholds);
		SignificanceRegisterObject(this,TEXT("Rider"), GetWorld());
	}
	SyncMoveComponent = Cast<USyncMoveComponent>(GetComponentByClass(USyncMoveComponent::StaticClass()));
}

void AARider::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	SignificanceUnregisterObject(this, GetWorld());
}

// Called every frame
void AARider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AARider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AARider::UpdateSignificanceSettings(const TMap<float, float>& SignificanceThresholdsMap)
{
	TArray<FSignificanceSettings::FSignificanceThresholds> Thresholds;
	for (auto Threshold : SignificanceThresholdsMap)
	{
		Thresholds.Emplace(Threshold.Key, Threshold.Value);
	}
	SetSignificanceSettings(Thresholds);
}


float AARider::SignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo,
                                    const FTransform& Viewpoint)
{
	if (ObjectInfo->GetTag() == TEXT("Rider"))
	{
		AARider* pawn = CastChecked<AARider>(ObjectInfo->GetObject());
		if (pawn)
		{
			if (!GetMesh()->WasRecentlyRendered(0.5f))
			{
				return -1;
			}
			const APlayerController* c = GetWorld()->GetFirstPlayerController();
			const float fovScale = c->PlayerCameraManager->GetFOVAngle() / c->PlayerCameraManager->DefaultFOV;
			float Distance = (pawn->GetActorLocation() - Viewpoint.GetLocation()).Size();
			Distance *= fovScale;
			return GetSignificanceByDistance(Distance);
		}
	}
	return 0;
}

void AARider::PostSignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo,
                                       float OldSignificance, float Significance, bool bFinal)
{
	if (ObjectInfo->GetTag() == TEXT("Rider"))
	{
		AARider* pawn = CastChecked<AARider>(ObjectInfo->GetObject());
		if (pawn)
		{
			USkeletalMeshComponent* mesh = GetMesh();
			if (mesh==nullptr)
			{
				return;
			}
			if (Significance == -1)
			{
				SetTickInterval(1.5);
				if (SyncMoveComponent)
				{
					SyncMoveComponent->SetComponentTickInterval(0.3);
				}
			}
			else if (Significance == 0)
			{
				SetTickInterval(1.5);
				mesh->SetComponentTickInterval(0.2);
			}
			else if (Significance == 1)
			{
				SetTickInterval(0.15);
				mesh->SetComponentTickInterval(0.15);
			}
			else if (Significance == 2)
			{
				SetTickInterval(0.05);
			}
			else if (Significance == 3)
			{
				SetTickInterval(1/30);
				mesh->SetComponentTickInterval(1/60);
			}
			if (mesh->GetAnimInstance()&&mesh->GetAnimInstance()->IsAnyMontagePlaying())
			{
				mesh->SetComponentTickInterval(1/60);
			}
		}
	}
}

void AARider::SetTickInterval(float interval)
{
	SetActorTickInterval(interval);
	for (auto Component : GetComponents())
	{
		if (Component)
		{
			Component->SetComponentTickInterval(interval);
		}
	}
	if (SyncMoveComponent)
	{
		SyncMoveComponent->SetComponentTickInterval(1/30);
	}
}

UAITask_MoveTo* AARider::MoveToLocation(FVector Position,float AcceptanceRadius, EAIOptionFlag::Type StopOnOverlap, EAIOptionFlag::Type AcceptPartialPath,
                                        bool bUsePathfinding, bool bLockAILogic, bool bUseContinuosGoalTracking, EAIOptionFlag::Type ProjectGoalOnNavigation)
{
    if(IsRunningDedicatedServer())
    {
    	MoveTo(Position,nullptr,AcceptanceRadius, StopOnOverlap, AcceptPartialPath, bUsePathfinding,  bLockAILogic,  bUseContinuosGoalTracking,  ProjectGoalOnNavigation);
    	return MoveToTaskPtr;
    }
    return nullptr;
}

UAITask_MoveTo* AARider::MoveToActor(AActor* ActorPtr,float AcceptanceRadius, EAIOptionFlag::Type StopOnOverlap, EAIOptionFlag::Type AcceptPartialPath,
    bool bUsePathfinding, bool bLockAILogic, bool bUseContinuosGoalTracking, EAIOptionFlag::Type ProjectGoalOnNavigation)
{
	if(IsValid(ActorPtr) && IsRunningDedicatedServer())
	{
		MoveTo(ActorPtr->GetActorLocation(),ActorPtr,AcceptanceRadius, StopOnOverlap, AcceptPartialPath, bUsePathfinding,  bLockAILogic,  bUseContinuosGoalTracking,  ProjectGoalOnNavigation);
		return MoveToTaskPtr;
	} 
	return nullptr;
}

void AARider::MoveTo(FVector InGoalLocation, AActor* InGoalActor, float AcceptanceRadius,
	EAIOptionFlag::Type StopOnOverlap, EAIOptionFlag::Type AcceptPartialPath, bool bUsePathfinding, bool bLockAILogic,
	bool bUseContinuousGoalTracking, EAIOptionFlag::Type ProjectGoalOnNavigation)
{
	if(AAIController* AIController = Cast<AAIController>(Controller))
	{
		if(MoveToTaskPtr == nullptr)
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
