// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomAbilitySystemGlobals.h"

#include "AbilitySystem/CustomGameplayAbility.h"
#include "CustomGameplayEffectTypes.h"
#include "AbilitySystem/CustomAbilitySystemComponent.h"
#include "Target/GameplayAbilityTargetData_SingleInt.h"

UCustomAbilitySystemGlobals* UCustomAbilitySystemGlobals::CustomGet()
{
	return Cast<UCustomAbilitySystemGlobals>(IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals());
}

void UCustomAbilitySystemGlobals::InitGlobalData()
{
	Super::InitGlobalData();
}

void UCustomAbilitySystemGlobals::InitGlobalTags()
{
	Super::InitGlobalTags();
	ShareCooldownTag = FGameplayTag::RequestGameplayTag("Ability.Skill.ShareCooldown");
}

FGameplayEffectContext* UCustomAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FCustomGameplayEffectContext();
}


