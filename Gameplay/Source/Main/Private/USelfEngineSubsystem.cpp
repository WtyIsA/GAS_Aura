// Fill out your copyright notice in the Description page of Project Settings.


#include "USelfEngineSubsystem.h"

#include "AbilitySystem/CustomAbilitySystemGlobals.h"
#include "AbilitySystem/CustomGameplayAbility.h"

void UUSelfEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UCustomAbilitySystemGlobals::CustomGet()->InitGlobalData();
}
