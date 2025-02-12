// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameplayAbilityTargetData_Msg.generated.h"

USTRUCT(BlueprintType)
struct MAIN_API FGameplayAbilityTargetData_Msg : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	FGameplayAbilityTargetData_Msg(): Data()
	{
	}

	FGameplayAbilityTargetData_Msg(const TArray<uint8>& MsgData): Data(MsgData)
	{
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Msg::StaticStruct();
	}

	UPROPERTY(EditAnywhere, Category = Targeting)
	TArray<TWeakObjectPtr<AActor>> TargetActorArray;
	
	UPROPERTY()
	FHitResult HitResult;
	
	bool bHasHitResult = false;

	virtual TArray<TWeakObjectPtr<AActor>> GetActors() const override
	{
		return TargetActorArray;
	}

	void AddActor(TWeakObjectPtr<AActor> Actor)
	{
		if (!TargetActorArray.Contains(Actor))
		{
			TargetActorArray.Push(Actor);
		}
	}

	void AddHitResult(FHitResult Result)
	{
		HitResult = Result;
		bHasHitResult = true;
		AddActor(HitResult.Actor);
	}

	virtual const FHitResult* GetHitResult() const override
	{
		return &HitResult;
	}

	virtual bool HasHitResult() const override
	{
		return bHasHitResult;
	}

	UPROPERTY()
	TArray<uint8> Data;
	
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template <>
struct TStructOpsTypeTraits<
		FGameplayAbilityTargetData_Msg> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Msg>
{
	enum
	{
		WithNetSerializer = true
		// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};
