// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CustomGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "CustomAbilitySystemGlobals.h"
#include "UnLuaBase.h"

UCustomGameplayAbility::UCustomGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

FString UCustomGameplayAbility::GetModuleName_Implementation() const
{
	return "";
}

FGameplayTag UCustomGameplayAbility::GetCooldownTag() const
{
	const FGameplayTagContainer* GameplayTagContainer = GetCooldownTags();
	return GameplayTagContainer ? GameplayTagContainer->First() : FGameplayTag();
}

bool UCustomGameplayAbility::LuaCommitAbility(bool InCreatePredictionWindow)
{
	UAbilitySystemComponent* AbilitySystemComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	if (InCreatePredictionWindow && !AbilitySystemComponent->ScopedPredictionKey.IsValidForMorePrediction())
	{
		if (CurrentActorInfo->IsLocallyControlled() && NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted)
		{
			FScopedPredictionWindow ScopedPredictionWindow(AbilitySystemComponent, true);
			return Super::K2_CommitAbility();
		}
	}
	
	return Super::K2_CommitAbility();
}

void UCustomGameplayAbility::GetCooldownTime(float& TimeRemaining, float& CooldownDuration) const
{
	Super::GetCooldownTimeRemainingAndDuration(CurrentSpecHandle, CurrentActorInfo, TimeRemaining, CooldownDuration);
}

void UCustomGameplayAbility::OnGameplayTaskInitialized(UGameplayTask& Task)
{
	Super::OnGameplayTaskInitialized(Task);
	for (TFieldIterator<FProperty> PropertyIt(Task.GetClass()); PropertyIt; ++PropertyIt)
	{
		FProperty* Property = *PropertyIt;
		if (Property && Property->IsA(FMulticastDelegateProperty::StaticClass()))
		{
			FMulticastDelegateProperty* DelegateProperty = Cast<FMulticastDelegateProperty>(Property);
			if (DelegateProperty)
			{
				FString DelegateName = DelegateProperty->GetName();
				OnFindTaskDelegate(&Task, DelegateName);
			}
		}
	}
}

void UCustomGameplayAbility::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
	OnTaskFinished(&Task);
}

void UCustomGameplayAbility::PostCDOContruct()
{
	UnLua::ClassBindLuaFile(this,false);
}

bool UCustomGameplayAbility::CheckCooldown(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           FGameplayTagContainer* OptionalRelevantTags) const
{
	UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	check(AbilitySystemComponent != nullptr);
	if (AbilitySystemComponent->HasMatchingGameplayTag(UCustomAbilitySystemGlobals::CustomGet()->ShareCooldownTag))
	{
		return false;
	}
	return Super::CheckCooldown(Handle, ActorInfo, OptionalRelevantTags);
}

void UCustomGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	LuaOnGiveAbility(ActorInfo ? *ActorInfo : FGameplayAbilityActorInfo(), Spec);
}

void UCustomGameplayAbility::AddShareCooldown() const
{
	if (CurrentActorInfo)
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (ASC)
		{
			ASC->UpdateTagMap(UCustomAbilitySystemGlobals::CustomGet()->ShareCooldownTag, 1);
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UCustomGameplayAbility:AddShareCooldown Error CurrentActorInfo Is Null "));
	}
}

void UCustomGameplayAbility::RemoveShareCooldown() const
{
	if (CurrentActorInfo)
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (ASC)
		{
			ASC->UpdateTagMap(UCustomAbilitySystemGlobals::CustomGet()->ShareCooldownTag, -1);
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UCustomGameplayAbility:RemoveShareCooldown Error CurrentActorInfo Is Null "));
	}
}

bool UCustomGameplayAbility::IsTaskFinished(UGameplayTask* Task)
{
	if (Task)
	{
		return Task->IsFinished();
	}
	return true;
}

FGameplayEffectSpecHandle UCustomGameplayAbility::GetCooldownGameplayEffect() const
{
	return LuaGetCooldownGameplayEffect();
}

FGameplayEffectSpecHandle UCustomGameplayAbility::GetCostGameplayEffect() const
{
	return LuaGetCostGameplayEffect();
}

FGameplayAbilitySpec UCustomGameplayAbility::K2_GetCurrentAbilitySpec() const
{
	return *GetCurrentAbilitySpec();
}

FGameplayAbilitySpecHandle UCustomGameplayAbility::K2_GetCurrentAbilitySpecHandle() const
{
	return GetCurrentAbilitySpecHandle();
}

FGameplayAbilityActorInfo UCustomGameplayAbility::K2_GetCurrentAbilityActorInfo() const
{
	return *GetCurrentActorInfo();
}

FGameplayAbilityActivationInfo UCustomGameplayAbility::K2_GetCurrentAbilityActivationInfo() const
{
	return GetCurrentActivationInfo();
}

void UCustomGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (TriggerEventData )
	{
		if (bHasBlueprintActivateFromEvent)
		{
			K2_ActivateAbilityFromEvent(*TriggerEventData);
			return;
		}
	}
	else if (bHasBlueprintActivate)
	{
		K2_ActivateAbility();
		return;
	}
	bool bReplicateEndAbility = false;
	bool bWasCancelled = true;
	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCustomGameplayAbility::SendTargetDataToServer(const FGameplayAbilityTargetDataHandle& TargetData, FGameplayTag ApplicationTag, bool bGenerateNewKey)
{
	if (IsPredictingClient())
	{
		UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
		check(ASC);

		FScopedPredictionWindow	ScopedPrediction(ASC, bGenerateNewKey);
		CurrentActorInfo->AbilitySystemComponent->CallServerSetReplicatedTargetData(CurrentSpecHandle,
			CurrentActivationInfo.GetActivationPredictionKey(), TargetData, ApplicationTag, ASC->ScopedPredictionKey);
	}
}

FGameplayEffectContextHandle UCustomGameplayAbility::K2_MakeEffectContext()
{
	return MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);
}
