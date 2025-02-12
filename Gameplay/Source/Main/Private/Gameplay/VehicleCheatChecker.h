// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCheatChecker.h"
#include "Components/ActorComponent.h"
#include "VehicleCheatChecker.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAIN_API UVehicleCheatChecker : public UBaseCheatChecker
{
	GENERATED_BODY()

public:
	UVehicleCheatChecker();
protected:
	
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
