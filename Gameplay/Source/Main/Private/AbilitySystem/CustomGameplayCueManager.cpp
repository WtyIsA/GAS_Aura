// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameplayCueManager.h"

#include "CustomAbilitySystemGlobals.h"
#include "GameplayCueSet.h"

FString UCustomGameplayCueManager::GetModuleName_Implementation() const
{
	return "Share.GAS.CustomGameplayCueMgr";
}

const TArray<FGameplayEffectCue>& UCustomGameplayCueManager::GetGameplayCueOfGESpec(const FGameplayEffectSpec& GESpec)
{
	return FindOrFetchGameplayCueOfGEId(GESpec.Id);
}

const TArray<FGameplayEffectCue>& UCustomGameplayCueManager::GetGameplayCueOfGESpec(const FGameplayEffectSpecForRPC& GESpec)
{
	return FindOrFetchGameplayCueOfGEId(GESpec.Id);
}

const TArray<FGameplayEffectCue>& UCustomGameplayCueManager::FindOrFetchGameplayCueOfGEId(int32 Id)
{
	TArray<FGameplayEffectCue>* GCList = GEIdToGCList.Find(Id);
	if (GCList == nullptr)
	{
		return GEIdToGCList.Add(Id, FetchGameplayCueListOfGEId(Id));
	}
	else
	{
		return *GCList;
	}
}
