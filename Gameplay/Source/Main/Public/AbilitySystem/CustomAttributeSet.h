
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/CustomAbilitySystemComponent.h"
#include "CustomAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// Delegate used to broadcast attribute events.
DECLARE_MULTICAST_DELEGATE_FiveParams(FCustomAttributeFourParamsEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec& /*EffectSpec*/, float /*EffectMagnitude*/,AActor* /*AvatarActor*/);

DECLARE_MULTICAST_DELEGATE_ThreeParams(FCustomAttributeThreeParamsEvent, FGameplayAttribute, float, float);
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(float, FCustomAttributeTwoParamsDelegate, const FGameplayAttribute&, Attribute, float&, NewValue);

/**
 * 
 */
UCLASS()
class MAIN_API UCustomAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UCustomAttributeSet();
	
	UCustomAbilitySystemComponent* GetCustomAbilitySystemComponent();
	
	mutable FCustomAttributeThreeParamsEvent OnAttributeChange;
	mutable FCustomAttributeTwoParamsDelegate PreAttributeChangeDelegate;
	UFUNCTION()
	FGameplayAbilityActorInfo K2_GetActorInfo();
	UFUNCTION()
	static FGameplayAttribute GetPropertyByName(const TSubclassOf<UAttributeSet> AttributeSetClass, FName InName);
};
