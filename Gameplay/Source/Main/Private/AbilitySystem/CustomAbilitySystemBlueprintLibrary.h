// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Target/GameplayAbilityTargetData_FireProjectileInfo.h"
#include "Target/GameplayAbilityTargetData_SingleInt.h"
#include "Target/GameplayAbilityTargetData_MeleeHitInfo.h"
#include "UObject/Object.h"
#include "CustomAbilitySystemBlueprintLibrary.generated.h"

UCLASS()
class MAIN_API UCustomAbilitySystemBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Target")
	static FGameplayAbilityTargetDataHandle  MakeAbilityTargetDataFromInt(int32 Data);

	UFUNCTION(BlueprintCallable, Category = "Target")
	static int ExtractIntFromAbilityTargetData(FGameplayAbilityTargetDataHandle handle, int32 index = 0);

	UFUNCTION(BlueprintCallable, Category = "Target")
	static FGameplayAbilityTargetDataHandle  MakeAbilityTargetDataFromMeleeHitInfo(FHitResult HitResult,FName SocketName);

	UFUNCTION(BlueprintCallable, Category = "Target")
	static FGameplayAbilityTargetData_MeleeHitInfo ExtractMeleeHitInfoFromAbilityTargetData(FGameplayAbilityTargetDataHandle handle, int32 index = 0);
	UFUNCTION(BlueprintCallable, Category = "Target")
	static FGameplayAbilityTargetDataHandle MakeAbilityTargetDataFromFireProjectileInfo(
		FHitResult HitResult, FVector InStartPos);
	UFUNCTION(BlueprintCallable, Category = "Target")
	static FGameplayAbilityTargetData_FireProjectileInfo ExtractFireProjectileInfoFromAbilityTargetData(
		FGameplayAbilityTargetDataHandle handle, int32 index);

	UFUNCTION(BlueprintCallable, Category = "Target")
	static FGameplayAbilityTargetDataHandle MakeGameplayAbilityTargetDataHandleFromHitResults(const TArray<FHitResult>& HitResults);
	
	UFUNCTION(BlueprintCallable, Category = "Target")
	static FGameplayAbilityTargetDataHandle MakeGameplayAbilityTargetDataHandleFromSingleHitResult(const FHitResult& HitResults);
	
	UFUNCTION(BlueprintCallable, Category = "Target")
	static FGameplayAbilityTargetDataHandle MakeGameplayAbilityTargetDataHandleFromActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Target")
	static FHitResult ExtractHitResultAbilityTargetData(FGameplayAbilityTargetDataHandle handle, int32 index = 0);

	UFUNCTION(BlueprintCallable, Category = "Target")
	static void SetGESpecContext(FGameplayEffectSpecHandle& SpecHandle, FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintCallable, Category = "Ability|EffectContext", Meta = (DisplayName = "GetTargetData"))
	static FGameplayAbilityTargetDataHandle EffectContextGetTargetData(FGameplayEffectContextHandle EffectContext);

	UFUNCTION(BlueprintCallable, Category = "Ability|EffectContext", Meta = (DisplayName = "AddTargetData"))
	static void EffectContextAddTargetData(FGameplayEffectContextHandle EffectContextHandle, const FGameplayAbilityTargetDataHandle& TargetData);

	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
	static void ClearTargetData(UPARAM(ref) FGameplayAbilityTargetDataHandle& TargetData);
	
	UFUNCTION()
	static FGameplayEffectSpecHandle MakeSpecHandle(int32 Id, UGameplayEffect* InGameplayEffect, AActor* InInstigator, AActor* InEffectCauser, float InLevel = 1.0f);

	UFUNCTION()
	static FGameplayEffectContext GetGEContext(FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION()
	static FGameplayEffectSpec GetGESpecFromSpecHandle(FGameplayEffectSpecHandle& SpecHandle);

	UFUNCTION()
	static FActiveGameplayEffectHandle GetActiveGEHandleFromActiveGE(FActiveGameplayEffect ActiveGE);

	UFUNCTION()
	static float GetSetByCallerMagnitudeFromGESpec(FGameplayEffectSpec EffectSpec, FGameplayTag DataTag,
	                                               float DefaultIfNotFound = 0);

	//index is 1,2,3,4...
	UFUNCTION()
	static FGameplayAbilityTargetData GetTargetDataFromHandle(FGameplayAbilityTargetDataHandle& Handle, int Index = 1);

	UFUNCTION()
	static void RemoveAllActiveGE_ClientCallOnly(UAbilitySystemComponent* ASC);
	UFUNCTION()
	static void ClearAllAbilities_ClientCallOnly(UAbilitySystemComponent* ASC);
	UFUNCTION()
	static void InitAttribute_ClientCallOnly(UCombatAttributeSet* InTargetAttributeSet, TMap<FString,float> InAttributes);
	UFUNCTION()
	static void AddAttributeSet_ClientCallOnly(UAbilitySystemComponent* ASC, UAttributeSet* InAttributeSet);
};
