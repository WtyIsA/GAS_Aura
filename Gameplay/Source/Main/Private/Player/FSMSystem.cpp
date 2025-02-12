// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FSMSystem.h"

// Sets default values for this component's properties
UFSMSystem::UFSMSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
	
}


// Called when the game starts
void UFSMSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFSMSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (CurState!=nullptr)
	{
		CurState->Tick(DeltaTime);
	}
}

void UFSMSystem::Start(UAnimInstance* animInstance,EStateType type)
{
	CurAnimInstance = animInstance;
	Print("FSM start"+GetTypeString(type));
	ChangeState(type);
}

void UFSMSystem::AddState(EStateType type, const TSubclassOf<UFSMState> StateClass)
{
	UFSMState* state = NewObject<UFSMState>(this,StateClass);
	state->StateType = type;
	state->Init(this);
	StateMap.Add(type,state);
	//Print("Add  "+GetTypeString(type));
}

void UFSMSystem::ChangeState(EStateType type, const TArray<void*>* args, EStateType nextType)
{
	UFSMState* old = CurState;
	if (!StateMap.Contains(type))
	{
		Print("!StateMap.Contains "+GetTypeString(type));
		return;
	}
	UFSMState* toState = StateMap[type];
	if (toState==nullptr||!toState->IsValidLowLevel())
	{
		Print("ToState is null "+GetTypeString(type));
		return;
	}
	if (!toState->PreEnter())
	{
		Print("PreEnter fail "+GetTypeString(type));
		return;
	}
	if (CurAnimInstance&&Cast<APawn>(CurAnimInstance->GetOwningActor())->IsLocallyControlled())
	{
	
		ServerChangeState(type,nextType);
	}
	if (old!=nullptr)
	{
		old->Leave(toState==nullptr?EStateType::Default:toState->StateType);
		Print("Leave:"+GetTypeString(old->StateType));
	}
	CurState = toState;
	toState->Enter(old==nullptr?EStateType::Default:old->StateType,args,nextType);
	Print("enter:"+GetTypeString(type));
}

void UFSMSystem::ChangeState(EStateType type, EStateType nextType)
{
	ChangeState(type,nullptr,nextType);
}

void UFSMSystem::ServerChangeState_Implementation(EStateType type,EStateType nextType)
{
	MultiCastChangeState(type,nextType);	
}


void UFSMSystem::MultiCastChangeState_Implementation(EStateType type,EStateType nextType)
{
	if (CurAnimInstance&&!Cast<APawn>(CurAnimInstance->GetOwningActor())->IsLocallyControlled())
	{
		ChangeState(type,nextType);
	}
}

void UFSMSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UFSMSystem::Print(FString s)
{
	UE_LOG(LogTemp,Warning,TEXT("[FSM]  %s"),*s);
}
