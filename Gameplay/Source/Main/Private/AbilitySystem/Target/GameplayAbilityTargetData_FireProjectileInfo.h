// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameplayAbilityTargetData_FireProjectileInfo.generated.h"

USTRUCT(BlueprintType)
struct MAIN_API FGameplayAbilityTargetData_FireProjectileInfo : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()
	FGameplayAbilityTargetData_FireProjectileInfo()
	{ }

	FGameplayAbilityTargetData_FireProjectileInfo(FHitResult InHitResult,FVector InStartPos)
		: HitResult(MoveTemp(InHitResult)),StartPos(InStartPos)
	{ }

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_FireProjectileInfo::StaticStruct();
	}

	UPROPERTY()
	FHitResult	HitResult;
	UPROPERTY()
	FVector StartPos;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template <>
struct TStructOpsTypeTraits<
		FGameplayAbilityTargetData_FireProjectileInfo> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_FireProjectileInfo>
{
	enum
	{
		WithNetSerializer = true
		// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};
