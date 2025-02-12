// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "CustomAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class MAIN_API UCustomAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY()
	FGameplayTag ShareCooldownTag;

	UFUNCTION()
	static UCustomAbilitySystemGlobals* CustomGet();

	virtual void InitGlobalData() override;
	virtual void InitGlobalTags() override;
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
