// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameplayAbilityTargetData_SingleInt.generated.h"

USTRUCT(BlueprintType)
struct MAIN_API FGameplayAbilityTargetData_SingleInt : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	FGameplayAbilityTargetData_SingleInt(): IntValue(0)
	{
	}

	FGameplayAbilityTargetData_SingleInt(int32 Value): IntValue(Value)
	{
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_SingleInt::StaticStruct();
	}

	UPROPERTY()
	int32 IntValue;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template <>
struct TStructOpsTypeTraits<
		FGameplayAbilityTargetData_SingleInt> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_SingleInt>
{
	enum
	{
		WithNetSerializer = true
		// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};
