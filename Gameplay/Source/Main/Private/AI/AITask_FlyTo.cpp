// Fill out your copyright notice in the Description page of Project Settings.


#include "AITask_FlyTo.h"

#include "Components/BoxComponent.h"

void UAITask_FlyTo::PerformMove()
{
	UPathFollowingComponent* PFComp = OwnerController ? OwnerController->GetPathFollowingComponent() : nullptr;
	if (PFComp == nullptr)
	{
		FinishMoveTask(EPathFollowingResult::Invalid);
		return;
	}

	ResetObservers();
	ResetTimers();

	if (PFComp->HasReached(MoveRequest))
	{
		MoveRequestID = PFComp->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
		OnRequestFinished(MoveRequestID, FPathFollowingResult(EPathFollowingResult::Success, FPathFollowingResultFlags::AlreadyAtGoal));
		return;
	}

	if (MoveRequest.IsMoveToActorRequest())
	{
		TargetActorLastLocation = FVector::ZeroVector;
		OwnerController->GetWorld()->GetTimerManager().SetTimer(UpdateActorLocationTimerHandle, this, &UAITask_FlyTo::OnUpdateActorLocation, UpdateActorLocationTickTime, true);
	}
	else
	{
		const APawn* Pawn = OwnerController->GetPawn();
		const FVector StartLocation = Pawn->GetActorLocation();
		UCPathAsyncFindPath* FlyPath = UCPathAsyncFindPath::FindPathAsync(GetVolume(), StartLocation, MoveRequest.GetGoalLocation());
		FlyPath->Success.AddDynamic(this, &UAITask_FlyTo::OnFindFlyPathSuccess);
		FlyPath->Failure.AddDynamic(this, &UAITask_FlyTo::OnFindFlyPathFail);
		FlyPath->Activate();
	}
}

void UAITask_FlyTo::OnFindFlyPathSuccess(const TArray<FCPathNode>& FlyPath, TEnumAsByte<ECPathfindingFailReason> FailReason)
{
	UPathFollowingComponent* PFComp = OwnerController ? OwnerController->GetPathFollowingComponent() : nullptr;
	if (PFComp == nullptr)
	{
		FinishMoveTask(EPathFollowingResult::Invalid);
		return;
	}
	TArray<FVector> Points;
	const APawn* Pawn = OwnerController->GetPawn();
	if(!Pawn || !IsValid(Pawn))
	{
		FinishMoveTask(EPathFollowingResult::Invalid);
		return;
	}
	const FVector StartLocation = Pawn->GetActorLocation();
	if (!GetWorld()->LineTraceTestByChannel(StartLocation, FlyPath[FlyPath.Num() - 1].WorldLocation, GetVolume()->TraceChannel))
	{
		Points.Add(FlyPath[0].WorldLocation);
		Points.Add(FlyPath[FlyPath.Num() - 1].WorldLocation);
	}
	else
	{
		for (FCPathNode Node : FlyPath)
		{
			Points.Add(Node.WorldLocation);
		}
	}
	if (Points.Num() < 2)
	{
		Points.Empty();
		Points.Add(StartLocation);
		Points.Add(TargetActorLastLocation);
	}
	FNavigationPath* NavPath = new FNavigationPath(Points);
	const FNavPathSharedPtr NavPathPtr(NavPath);
	MoveRequestID = OwnerController->RequestMove(MoveRequest, NavPathPtr);
	PathFinishDelegateHandle = PFComp->OnRequestFinished.AddUObject(this, &UAITask_FlyTo::OnRequestFinished);
	SetObservedPath(NavPathPtr);
}

void UAITask_FlyTo::OnFindFlyPathFail(const TArray<FCPathNode>& FlyPath, TEnumAsByte<ECPathfindingFailReason> FailReason)
{
	ResetTimers();
	FinishMoveTask(EPathFollowingResult::Invalid);
}

void UAITask_FlyTo::OnUpdateActorLocation()
{
	const AActor* TargetActor = MoveRequest.GetGoalActor();
	if (IsValid(TargetActor))
	{
		FVector TargetActorCurrentLocation = TargetActor->GetActorLocation();
		TargetActorCurrentLocation.Z += UKismetMathLibrary::RandomFloatInRange(10, 30);
		UPathFollowingComponent* PFComp = OwnerController ? OwnerController->GetPathFollowingComponent() : nullptr;
		if (PFComp == nullptr)
		{
			FinishMoveTask(EPathFollowingResult::Invalid);
			return;
		}
		if (PFComp->HasReached(MoveRequest))
		{
			MoveRequestID = PFComp->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
			OnRequestFinished(MoveRequestID, FPathFollowingResult(EPathFollowingResult::Success, FPathFollowingResultFlags::AlreadyAtGoal));
			GetWorld()->GetTimerManager().ClearTimer(UpdateActorLocationTimerHandle);
			return;
		}
		if (!(TargetActorCurrentLocation - TargetActorLastLocation).IsNearlyZero(100))
		{
			TargetActorLastLocation = TargetActorCurrentLocation;
			const APawn* Pawn = OwnerController->GetPawn();
			if (!IsValid(Pawn))
			{
				FinishMoveTask(EPathFollowingResult::Invalid);
				return;
			}
			const FVector StartLocation = Pawn->GetActorLocation();
			UCPathAsyncFindPath* FlyPath = UCPathAsyncFindPath::FindPathAsync(GetVolume(), StartLocation, TargetActorLastLocation);
			FlyPath->Success.AddDynamic(this, &UAITask_FlyTo::OnFindFlyPathSuccess);
			FlyPath->Failure.AddDynamic(this, &UAITask_FlyTo::OnFindFlyPathFail);
			FlyPath->Activate();
		}
	}
}

ACPathVolume* UAITask_FlyTo::GetVolume()
{
	if (Volume && IsValid(Volume))
	{
		return Volume;
	}
	const APawn* Pawn = OwnerController->GetPawn();
	TArray<ACPathVolume*>* Volumes = ULuaManger::Get()->GetFlyPathVolumes();
	if (Volumes->Num() < 1)
	{
		Volumes->Add(NewObject<ACPathVolume>());
	}
	if (!IsValid(Pawn))
	{
		return (*Volumes)[0];
	}
	const FVector Location = Pawn->GetActorLocation();
	for (ACPathVolume* InVolume : *Volumes)
	{
		const UBoxComponent* Box = InVolume->VolumeBox;
		if (Box->Bounds.GetBox().IsInside(Location))
		{
			Volume = InVolume;
			return Volume;
		}
	}
	return (*Volumes)[0];
}
