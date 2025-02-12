// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FSMState.h"

#include <string>
#include "Player/FSMSystem.h"
#include "Player/MyShootCharacter.h"

UFSMState::UFSMState()
{
	
}

void UFSMState::Init(UFSMSystem* fsm)
{
	FSM = fsm;
	ActorOwner = fsm->GetOwner();
	FSM->Print(ActorOwner->GetName());
}

bool UFSMState::PreEnter()
{
	return true;
}

void UFSMState::Enter(EStateType fromState, const TArray<void*>*, EStateType nextState)
{
	FromState = fromState;
	NextState = nextState;
	BlendingOutDelegate.BindUObject(this,&UFSMState::OnMontageFinish);
	EndDelegate.BindUObject(this,&UFSMState::OnMontageFinish);
}

void UFSMState::Tick(float deltaTime)
{
	
}

void UFSMState::Leave(EStateType toState)
{
}

void UFSMState::OnMontageFinish(UAnimMontage* Montage, bool bInterrupted)
{
	
}

void UFSMState::BindMontageEndDelegate(bool blendOut)
{
	if (blendOut)
	{
		FSM->CurAnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate);
	}
	else
	{	
		FSM->CurAnimInstance->Montage_SetEndDelegate(EndDelegate);
	}
}





