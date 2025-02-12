// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "UnLuaInterface.h"
#include "UObject/Object.h"
#include "CustomGameplayCueManager.generated.h"

UCLASS()
class MAIN_API UCustomGameplayCueManager : public UGameplayCueManager, public IUnLuaInterface
{
	
public:
	
	GENERATED_BODY()
	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override
	{
		//return true; // Default  
		return false;
	}

	virtual FString GetModuleName_Implementation() const override;
	
	virtual const TArray<FGameplayEffectCue>& GetGameplayCueOfGESpec(const FGameplayEffectSpec& GESpec) override;
	virtual const TArray<FGameplayEffectCue>& GetGameplayCueOfGESpec(const FGameplayEffectSpecForRPC& GESpec) override;

	UFUNCTION(BlueprintImplementableEvent)
	TArray<FGameplayEffectCue> FetchGameplayCueListOfGEId(int32 GEId);

private:
	TMap<int32, TArray<FGameplayEffectCue>> GEIdToGCList;

private:
	const TArray<FGameplayEffectCue>& FindOrFetchGameplayCueOfGEId(int32 Id);
};
