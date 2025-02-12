// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameplayAbilityTargetData_MeleeHitInfo.generated.h"

USTRUCT(BlueprintType)
struct MAIN_API FGameplayAbilityTargetData_MeleeHitInfo : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()
	FGameplayAbilityTargetData_MeleeHitInfo()
	{ }

	FGameplayAbilityTargetData_MeleeHitInfo(FHitResult InHitResult,FName InSocketName)
		: HitResult(MoveTemp(InHitResult)),SocketName(InSocketName)
	{ }

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_MeleeHitInfo::StaticStruct();
	}

	UPROPERTY()
	FHitResult	HitResult;
	UPROPERTY()
	FName SocketName;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template <>
struct TStructOpsTypeTraits<
		FGameplayAbilityTargetData_MeleeHitInfo> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_MeleeHitInfo>
{
	enum
	{
		WithNetSerializer = true
		// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};
