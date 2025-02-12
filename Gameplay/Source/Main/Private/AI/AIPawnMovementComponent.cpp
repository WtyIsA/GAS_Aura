#include "AIPawnMovementComponent.h"
#include "AIController.h"
#include "AITypes.h"
#include "NavigationData.h"
#include "GameFramework/Pawn.h"

void UAIPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent || !HasTarget)
	{
		return;
	}
	if(!AIControllerPtr.IsValid())
	{
		FindController();
		if (!AIControllerPtr.IsValid())
		{
			return;
		}
	}
	CurrentTimeout -= DeltaTime;
	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	Velocity = (CurrentTargetPosition - OldLocation) / DeltaTime;
	if (IsExceedingMaxSpeed(MaxSpeed))
	{
		Velocity = Velocity.GetSafeNormal() * MaxSpeed;
	}
	if(bFinishRotate == false)
	{
		RotateTo(Velocity.ToOrientationRotator());
		bFinishRotate = true;
	}
	FVector Delta = Velocity * DeltaTime;
	if(PrintDebugLog)
	{
		UE_LOG(LogNet, Warning, TEXT("UAIPawnMovementComponent::Velocity=%s,Delta=%s"),*Velocity.ToString(),*Delta.ToString());
	}
	const bool MovePassGoal = Delta.Size() >= FVector::Distance(CurrentTargetPosition,OldLocation);
	if(MovePassGoal && CurrentPathIndex == PathPoints.Num() - 1)
	{
		Delta = CurrentTargetPosition - OldLocation;
	}
	if (!Delta.IsNearlyZero(1e-6f))
	{
		const FQuat OldRotation = UpdatedComponent->GetComponentQuat();
		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Delta, OldRotation, true, Hit);
		if (Hit.IsValidBlockingHit())
		{
			HandleImpact(Hit, DeltaTime, Delta);
			SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);
		}else if(Hit.bStartPenetrating)
		{
			ResolvePenetration(GetPenetrationAdjustment(Hit), Hit, UpdatedComponent->GetComponentQuat());
		}
	}
	const FVector NewLocation = UpdatedComponent->GetComponentLocation();
	Velocity = ((NewLocation - OldLocation) / DeltaTime);
	
	UpdateComponentVelocity();
	bool MoveToNextPoint = false;
	if(FVector::Dist2D(NewLocation,CurrentTargetPosition) <= AcceptableRadius || MovePassGoal)
	{
		MoveToNextPoint = true;
	}
	bool IsAtGoal = MoveToNextPoint && CurrentPathIndex == PathPoints.Num() -1;
	if(MoveToNextPoint && !IsAtGoal)
	{
		++CurrentPathIndex;
		CurrentTargetPosition = PathPoints[CurrentPathIndex].Location;
		bFinishRotate = false;
	}
	if(PrintDebugLog)
	{
		UE_LOG(LogNet, Warning, TEXT("UAIPawnMovementComponent::OldLocation=%s,NewLocation=%s,CurrentTarge=%s"),*OldLocation.ToString(),*NewLocation.ToString(),*CurrentTargetPosition.ToString());
		UE_LOG(LogNet, Warning, TEXT("UAIPawnMovementComponent::CurrentPathIndex=%d,MoveToNextPoint=%d,IsAtGoal=%d,CurrentTimeout=%f"),CurrentPathIndex,MoveToNextPoint,IsAtGoal,CurrentTimeout);
	}
	if(CurrentTimeout > 0 && IsAtGoal && bChaseTarget && DestinationActor.IsValid())
	{
		float Distance = FVector::Distance(NewLocation,DestinationActor->GetActorLocation());
		if(Distance > AcceptableRadius)
		{
			CurrentTargetPosition = DestinationActor->GetActorLocation();
			SetInitParams(CurrentTimeout,bTeleportAfterTimeout);
			IsAtGoal = false;
		}
	}
	if(CurrentTimeout <= 0 || IsAtGoal)
	{
		OnMovementStatusChange.ExecuteIfBound(IsAtGoal ? 0 : 1);
		HasTarget = false;
	}
}

void UAIPawnMovementComponent::MoveToLocation(FVector TargetPosition, int Timeout, bool TeleportAfterTimeout)
{
	ResetParams();
	FindController();
	bChaseTarget = false;
	CurrentTargetPosition = TargetPosition;
	SetInitParams(Timeout,TeleportAfterTimeout);
}

void UAIPawnMovementComponent::MoveToActor(const AActor* TargetActor, int Timeout, bool TeleportAfterTimeout)
{
	ResetParams();
	FindController();
	bChaseTarget = true;
	DestinationActor = MakeWeakObjectPtr(const_cast<AActor*>(TargetActor));
	CurrentTargetPosition = TargetActor->GetActorLocation();
	SetInitParams(Timeout,TeleportAfterTimeout);
}

void UAIPawnMovementComponent::LookAt(FVector TargetPosition)
{
	RotateTo(FVector(TargetPosition - UpdatedComponent->GetComponentLocation()).ToOrientationRotator());
}

void UAIPawnMovementComponent::StopMoveNow()
{
	ResetParams();
	OnMovementStatusChange.Unbind();
}

void UAIPawnMovementComponent::RotateTo(FRotator TargetRotator)
{
	const FRotator OldRotator = PawnOwner->GetActorRotation();
	const FRotator RealRotator = FRotator(OldRotator.Pitch,TargetRotator.Yaw,OldRotator.Roll);
	PawnOwner->SetActorRotation(RealRotator,ETeleportType::TeleportPhysics);
	if(PrintDebugLog)
	{
		UE_LOG(LogNet, Warning, TEXT("UAIPawnMovementComponent::RealRotator=%s"),*RealRotator.ToString());
		UE_LOG(LogNet, Warning, TEXT("UAIPawnMovementComponent::ActorRotator=%s"),*(PawnOwner->GetActorRotation().ToString()));
	}
}

void UAIPawnMovementComponent::ResetParams()
{
	bChaseTarget = false;
	bFinishRotate = false;
	bIsLookAt = false;
	HasTarget = false;
	bStuck = false;
	CurrentTimeout = 0;
	PathPoints.Reset();
	CurrentPathIndex = 0;
}

void UAIPawnMovementComponent::SetInitParams(float Timeout, bool TeleportAfterTimeout)
{
	if(Timeout > 0)
	{
		CurrentTimeout = Timeout;
	}else
	{
		const float Distance = FVector::Distance(GetPawnOwner()->GetActorLocation(),CurrentTargetPosition);
		CurrentTimeout = Distance / MaxSpeed;
	}
	bTeleportAfterTimeout = TeleportAfterTimeout;
	if(!FindPath(true))
	{
		FindPath(false);
	}
	if(PathPoints.Num() > 0)
	{
		HasTarget = true;
		CurrentPathIndex = 0;
		CurrentTargetPosition = PathPoints[0].Location;
		if(PrintDebugLog)
		{
			UE_LOG(LogNet, Warning, TEXT("UAIPawnMovementComponent::SetInitParams() FirstTarget=%s,CurrentTimeout=%f"),*CurrentTargetPosition.ToString(),CurrentTimeout);
		}
	}else
	{
		//TODO
		HasTarget = false;
	}
}

void UAIPawnMovementComponent::FindController()
{
	if(!AIControllerPtr.IsValid() && PawnOwner)
	{
		AController* Controller = PawnOwner->GetController();
		if(Controller)
		{
			AIControllerPtr = MakeWeakObjectPtr(static_cast<AAIController*>(Controller));
		}
	}
}

bool UAIPawnMovementComponent::FindPath(bool UsePathFinding)
{
	FAIMoveRequest MoveReq;
	MoveReq.SetAllowPartialPath(bAllowPartialPath);
	MoveReq.SetAcceptanceRadius(AcceptableRadius);
	MoveReq.SetCanStrafe(bAllowStrafe);
	MoveReq.SetUsePathfinding(UsePathFinding);
	if(DestinationActor.IsValid())
	{
		MoveReq.SetGoalActor(DestinationActor.Get());
	}else
	{
		MoveReq.SetGoalLocation(CurrentTargetPosition);
	}
	FPathFindingQuery PFQuery;
	if(AIControllerPtr.IsValid() && AIControllerPtr->BuildPathfindingQuery(MoveReq,PFQuery))
	{
		FNavPathSharedPtr Path;
		AIControllerPtr->FindPathForMoveRequest(MoveReq,PFQuery,Path);
		if(Path.IsValid())
		{
			PathPoints = Path->GetPathPoints();
		}
	}
	return PathPoints.Num() > 0;
}
