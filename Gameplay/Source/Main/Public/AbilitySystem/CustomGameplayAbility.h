// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "Abilities/GameplayAbility.h"
#include "UObject/Object.h"
#include "CustomGameplayAbility.generated.h"

UCLASS()
class MAIN_API UCustomGameplayAbility : public UGameplayAbility, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	UCustomGameplayAbility();

	virtual FString GetModuleName_Implementation() const override;
	
	UFUNCTION()
	FGameplayTag GetCooldownTag() const;

	UFUNCTION()
	bool LuaCommitAbility(bool InCreatePredictionWindow = true);
	
	UFUNCTION()
	void GetCooldownTime(float& TimeRemaining, float& CooldownDuration) const;
	
	virtual void OnGameplayTaskInitialized(UGameplayTask& Task) override;

	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;

	virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintImplementableEvent)
	void LuaOnGiveAbility(const FGameplayAbilityActorInfo ActorInfo, const FGameplayAbilitySpec& Spec);
	
	UFUNCTION()
	void AddShareCooldown() const;
	
	UFUNCTION()
	void RemoveShareCooldown() const;

	virtual void PostCDOContruct() override;

	UFUNCTION()
	bool IsTaskFinished(UGameplayTask* Task);
	
	// Sends TargetData from the client to the Server and creates a new Prediction Window
	UFUNCTION(BlueprintCallable)
	virtual void SendTargetDataToServer(const FGameplayAbilityTargetDataHandle& TargetData ,FGameplayTag ApplicationTag, bool bGenerateNewKey = false);

	/** Returns the gameplay effect used to determine cooldown */
	virtual FGameplayEffectSpecHandle GetCooldownGameplayEffect() const override;
	UFUNCTION(BlueprintImplementableEvent)
	FGameplayEffectSpecHandle LuaGetCooldownGameplayEffect() const;

	/** Returns the gameplay effect used to apply cost */
	virtual FGameplayEffectSpecHandle GetCostGameplayEffect() const override;
	UFUNCTION(BlueprintImplementableEvent)
	FGameplayEffectSpecHandle LuaGetCostGameplayEffect() const;

	UFUNCTION()
	FGameplayAbilitySpec K2_GetCurrentAbilitySpec() const;

	UFUNCTION()
	FGameplayAbilitySpecHandle K2_GetCurrentAbilitySpecHandle() const;

	UFUNCTION()
 	FGameplayAbilityActorInfo K2_GetCurrentAbilityActorInfo() const;

	UFUNCTION()
	FGameplayAbilityActivationInfo K2_GetCurrentAbilityActivationInfo() const;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnFindTaskDelegate(UGameplayTask* GameplayTask, const FString& DelegateName);

	UFUNCTION(BlueprintImplementableEvent)
	void OnTaskFinished(UGameplayTask* GameplayTask);

	UFUNCTION()
	FGameplayEffectContextHandle K2_MakeEffectContext();
};
