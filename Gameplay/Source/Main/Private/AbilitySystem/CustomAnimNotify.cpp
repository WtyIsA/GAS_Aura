// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CustomAnimNotify.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystem/CustomAbilitySystemComponent.h"

void UCustomAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	CurAnimation = Animation;
	AActor* actor = MeshComp->GetOwner();
	const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(actor);
	if (AbilitySystemInterface)
	{
		const UCustomAbilitySystemComponent* CustomAbilitySystemComponent = Cast<UCustomAbilitySystemComponent>(
			AbilitySystemInterface->GetAbilitySystemComponent());
		if (CustomAbilitySystemComponent)
		{
			CustomAbilitySystemComponent->AnimNotifyDelegate.Broadcast(this);
		}
	}
}
