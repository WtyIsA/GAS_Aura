// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <string>


#include "EStateType.h"
#include "Animation/AnimInstance.h"

#include "FSMState.generated.h"

class UFSMSystem;
UCLASS( ClassGroup=(Custom), Blueprintable, BlueprintType, HideCategories = ("Cooking", "LOD", "Physics",
	"Activation", "Tags", "Rendering"))
class  UFSMState : public UObject
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFSMState();
	UPROPERTY()
    UFSMSystem* FSM;
	UPROPERTY()
	AActor* ActorOwner;
	EStateType StateType;
	
	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded EndDelegate;
	
	virtual void Init(UFSMSystem* fsm);
	virtual bool PreEnter();
	virtual void Enter(EStateType fromState, const TArray<void*>* args, EStateType nextState);
	virtual void Tick(float deltaTime);
	virtual void Leave(EStateType toState);
	virtual void OnMontageFinish(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void BindMontageEndDelegate(bool blendOut=true);

protected:
	EStateType FromState;
	EStateType NextState;
public:
};
