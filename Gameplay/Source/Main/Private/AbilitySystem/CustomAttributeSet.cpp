#include "AbilitySystem/CustomAttributeSet.h"
#include "Player/MyShootCharacter.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "SelfPlayerController.h"

UCustomAttributeSet::UCustomAttributeSet()
{
}

UCustomAbilitySystemComponent* UCustomAttributeSet::GetCustomAbilitySystemComponent()
{
	return Cast<UCustomAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

FGameplayAbilityActorInfo UCustomAttributeSet::K2_GetActorInfo()
{
	FGameplayAbilityActorInfo* ActorInfo = GetActorInfo();
	if (ActorInfo)
	{
		return  *GetActorInfo();
	}
	else
	{
		return FGameplayAbilityActorInfo();
	}
}

FGameplayAttribute UCustomAttributeSet::GetPropertyByName(const TSubclassOf<UAttributeSet> AttributeSetClass,FName InName)
{
	FProperty* Field = FindFieldChecked<FProperty>(AttributeSetClass, InName);
	return FGameplayAttribute(Field);
}
