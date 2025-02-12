// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/AITask_MoveTo.h"
#include "AIController.h"
#include "CPathNode.h"
#include "CPathFindPath.h"
#include "LuaManger.h"
#include "Kismet/KismetMathLibrary.h"
#include "AITask_FlyTo.generated.h"

UCLASS()
class MAIN_API UAITask_FlyTo : public UAITask_MoveTo
{
	GENERATED_BODY()
private:
	virtual void PerformMove() override;

	UFUNCTION()
	void OnFindFlyPathSuccess(const TArray<FCPathNode>& Path, TEnumAsByte<ECPathfindingFailReason> FailReason);

	UFUNCTION()
	void OnFindFlyPathFail(const TArray<FCPathNode>& Path, TEnumAsByte<ECPathfindingFailReason> FailReason);

	float UpdateActorLocationTickTime = 1.0;

	FTimerHandle UpdateActorLocationTimerHandle;

	FVector TargetActorLastLocation;

	UFUNCTION()
	void OnUpdateActorLocation();

	ACPathVolume* GetVolume();

	UPROPERTY()
	ACPathVolume* Volume;
};
