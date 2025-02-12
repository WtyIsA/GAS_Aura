// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "UnLuaInterface.h"
#include "AbilitySystem/Attributes/CombatAttributeSet.h"
#include "CustomCalculation.generated.h"

UCLASS()
class MAIN_API UCustomCalculation : public UGameplayModMagnitudeCalculation, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	virtual FString GetModuleName_Implementation() const override;

	virtual void PostCDOContruct() override;
	
protected:
	
	UFUNCTION()
	float GetAttributeCaptureSpec(const FGameplayEffectSpec& Spec, const TSubclassOf<UCustomAttributeSet> AttributeSetClass, const FString& InName, const EGameplayEffectAttributeCaptureSource InType);
	
	UFUNCTION()
	float GetSetByCaller(const FGameplayEffectSpec& Spec, FName& InName, float InDefaultValue);

	UFUNCTION()
	float GetTagSetByCaller(const FGameplayEffectSpec& Spec, FGameplayTag& InTag, float InDefaultValue);
};
