// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCheatChecker.h"
#include "Components/CapsuleComponent.h"
#include "CharacterCheatChecker.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAIN_API UCharacterCheatChecker : public UBaseCheatChecker
{
	GENERATED_BODY()

public:
	UCharacterCheatChecker();
	virtual FVector GetGroundPos(FVector pos,bool& hasGround) override;
protected:
	virtual void BeginPlay() override;
	const UCapsuleComponent* CapsuleComponent;
	float LastCollisionRadius;
public:
	virtual void SelfPullBacked() override;
	virtual	bool IsCollisionRadiusChange() override;
	virtual	bool IsTargetPosFly(FVector targetPos,EStateType state) override;
	virtual	bool IsClimbPosOk(FVector targetPos,EStateType state) override;
	virtual	bool IsLadderPosOk(FVector targetPos,EStateType state) override;
	virtual	bool IsCurvePosOk(FVector targetPos,EStateType state, double timestamp) override;
	virtual bool PassSwimStateCheck(EStateType state) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
