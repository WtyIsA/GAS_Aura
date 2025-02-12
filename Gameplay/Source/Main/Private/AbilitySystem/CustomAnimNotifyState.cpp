// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CustomAnimNotifyState.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/CustomAbilitySystemComponent.h"

void UCustomAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
										 float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	CurAnimation = Animation;
	Owner =MeshComp->GetOwner();
	ProcessEventByName("Begin");
	
}

void UCustomAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
										float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	CurAnimation = Animation;
}

void UCustomAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	CurAnimation = Animation;
	Owner =MeshComp->GetOwner();
	ProcessEventByName("End");
}

void UCustomAnimNotifyState::ProcessEventByName(const FName& EventName)
{
	if (Owner)
	{
		const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Owner);
		if (AbilitySystemInterface)
		{
			const UCustomAbilitySystemComponent* CustomAbilitySystemComponent = Cast<UCustomAbilitySystemComponent>(
				AbilitySystemInterface->GetAbilitySystemComponent());
			if (CustomAbilitySystemComponent)
			{
				CustomAbilitySystemComponent->AnimNotifyStateDelegate.Broadcast(this, EventName);
			}
		}
	}
}
