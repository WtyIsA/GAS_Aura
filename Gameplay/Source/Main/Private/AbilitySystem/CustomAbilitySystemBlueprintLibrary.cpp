// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomAbilitySystemBlueprintLibrary.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CustomGameplayEffectTypes.h"
#include "Attributes/CombatAttributeSet.h"
#include "Target/GameplayAbilityTargetData_FireProjectileInfo.h"
#include "Target/GameplayAbilityTargetData_MeleeHitInfo.h"
#include "Target/GameplayAbilityTargetData_SingleInt.h"

FGameplayAbilityTargetDataHandle UCustomAbilitySystemBlueprintLibrary::MakeAbilityTargetDataFromInt(int32 Data)
{
	return FGameplayAbilityTargetDataHandle(new FGameplayAbilityTargetData_SingleInt(Data));
}

int32 UCustomAbilitySystemBlueprintLibrary::ExtractIntFromAbilityTargetData(FGameplayAbilityTargetDataHandle handle, int32 index)
{
	const FGameplayAbilityTargetData_SingleInt* TargetData = static_cast<FGameplayAbilityTargetData_SingleInt*>(handle.Get(index));
	return TargetData ? TargetData->IntValue : 0;
}

FGameplayAbilityTargetDataHandle UCustomAbilitySystemBlueprintLibrary::MakeAbilityTargetDataFromMeleeHitInfo(
	FHitResult HitResult, FName SocketName)
{
	return FGameplayAbilityTargetDataHandle(new FGameplayAbilityTargetData_MeleeHitInfo(HitResult,SocketName));
}

FGameplayAbilityTargetData_MeleeHitInfo UCustomAbilitySystemBlueprintLibrary::ExtractMeleeHitInfoFromAbilityTargetData(
	FGameplayAbilityTargetDataHandle handle, int32 index)
{
	const FGameplayAbilityTargetData_MeleeHitInfo* TargetData = static_cast<FGameplayAbilityTargetData_MeleeHitInfo*>(handle.Get(index));
	return *TargetData;
}

FGameplayAbilityTargetDataHandle UCustomAbilitySystemBlueprintLibrary::MakeAbilityTargetDataFromFireProjectileInfo(
	FHitResult HitResult, FVector InStartPos)
{
	return FGameplayAbilityTargetDataHandle(new FGameplayAbilityTargetData_FireProjectileInfo(HitResult,InStartPos));
}

FGameplayAbilityTargetData_FireProjectileInfo UCustomAbilitySystemBlueprintLibrary::ExtractFireProjectileInfoFromAbilityTargetData(
	FGameplayAbilityTargetDataHandle handle, int32 index)
{
	const FGameplayAbilityTargetData_FireProjectileInfo* TargetData = static_cast<FGameplayAbilityTargetData_FireProjectileInfo*>(handle.Get(index));
	return *TargetData;
}

FGameplayAbilityTargetDataHandle UCustomAbilitySystemBlueprintLibrary::MakeGameplayAbilityTargetDataHandleFromHitResults(const TArray<FHitResult>& HitResults)
{
	FGameplayAbilityTargetDataHandle TargetData;

	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	return TargetData;
}

FGameplayAbilityTargetDataHandle UCustomAbilitySystemBlueprintLibrary::MakeGameplayAbilityTargetDataHandleFromSingleHitResult(const FHitResult& HitResult)
{
	return FGameplayAbilityTargetDataHandle(new FGameplayAbilityTargetData_SingleTargetHit(HitResult));
}

FGameplayAbilityTargetDataHandle UCustomAbilitySystemBlueprintLibrary::MakeGameplayAbilityTargetDataHandleFromActor(AActor* Actor)
{
	const FHitResult HitResult(Actor, Cast<UPrimitiveComponent>(Actor->K2_GetRootComponent()), Actor->K2_GetActorLocation(), Actor->K2_GetActorLocation());
	return FGameplayAbilityTargetDataHandle(new FGameplayAbilityTargetData_SingleTargetHit(HitResult));
}

FHitResult UCustomAbilitySystemBlueprintLibrary::ExtractHitResultAbilityTargetData(FGameplayAbilityTargetDataHandle handle, int32 index)
{
	const FGameplayAbilityTargetData_SingleTargetHit* TargetData = static_cast<FGameplayAbilityTargetData_SingleTargetHit*>(handle.Get(index));
	return TargetData ? TargetData->HitResult : FHitResult();
}

void UCustomAbilitySystemBlueprintLibrary::SetGESpecContext(FGameplayEffectSpecHandle& SpecHandle, FGameplayEffectContextHandle& ContextHandle)
{
	SpecHandle.Data->SetContext(ContextHandle);
}

FGameplayAbilityTargetDataHandle UCustomAbilitySystemBlueprintLibrary::EffectContextGetTargetData(FGameplayEffectContextHandle EffectContextHandle)
{
	FCustomGameplayEffectContext* EffectContext = static_cast<FCustomGameplayEffectContext*>(EffectContextHandle.Get());
	
	if (EffectContext)
	{
		return EffectContext->GetTargetData();
	}

	return FGameplayAbilityTargetDataHandle();
}

void UCustomAbilitySystemBlueprintLibrary::EffectContextAddTargetData(FGameplayEffectContextHandle EffectContextHandle, const FGameplayAbilityTargetDataHandle& TargetData)
{
	FCustomGameplayEffectContext* EffectContext = static_cast<FCustomGameplayEffectContext*>(EffectContextHandle.Get());

	if (EffectContext)
	{
		EffectContext->AddTargetData(TargetData);
	}
}

void UCustomAbilitySystemBlueprintLibrary::ClearTargetData(FGameplayAbilityTargetDataHandle& TargetData)
{
	TargetData.Clear();
}

FGameplayEffectSpecHandle UCustomAbilitySystemBlueprintLibrary::MakeSpecHandle(int32 Id, UGameplayEffect* InGameplayEffect, AActor* InInstigator, AActor* InEffectCauser, float InLevel)
{
	FGameplayEffectContext* EffectContext = UAbilitySystemGlobals::Get().AllocGameplayEffectContext();
	EffectContext->AddInstigator(InInstigator, InEffectCauser);
	FGameplayEffectSpec* Spec = new FGameplayEffectSpec(InGameplayEffect, FGameplayEffectContextHandle(EffectContext), InLevel);
	Spec->Id = Id;
	return FGameplayEffectSpecHandle(Spec);

}

FGameplayEffectContext UCustomAbilitySystemBlueprintLibrary::GetGEContext(FGameplayEffectContextHandle& ContextHandle)
{
	FGameplayEffectContext* GEContext = ContextHandle.Get();
	if (GEContext)
	{
		return *GEContext;
	}
	return FGameplayEffectContext();
}

FGameplayEffectSpec UCustomAbilitySystemBlueprintLibrary::GetGESpecFromSpecHandle(FGameplayEffectSpecHandle& SpecHandle)
{
	if (SpecHandle.Data.IsValid())
	{
		return *SpecHandle.Data.Get();
	}
	else
	{
		return FGameplayEffectSpec();
	}
}

FActiveGameplayEffectHandle UCustomAbilitySystemBlueprintLibrary::GetActiveGEHandleFromActiveGE(FActiveGameplayEffect ActiveGE)
{
	return ActiveGE.Handle;
}

float UCustomAbilitySystemBlueprintLibrary::GetSetByCallerMagnitudeFromGESpec(FGameplayEffectSpec EffectSpec,
                                                                              FGameplayTag DataTag,
                                                                              float DefaultIfNotFound)
{
	return EffectSpec.GetSetByCallerMagnitude(DataTag, false, DefaultIfNotFound);
}

FGameplayAbilityTargetData UCustomAbilitySystemBlueprintLibrary::GetTargetDataFromHandle(FGameplayAbilityTargetDataHandle& Handle, int Index)
{
	FGameplayAbilityTargetData* TargetData = Handle.Get(Index - 1);
	if (TargetData)
	{
		return *TargetData;
	}
	return FGameplayAbilityTargetData();
}

void UCustomAbilitySystemBlueprintLibrary::RemoveAllActiveGE_ClientCallOnly(UAbilitySystemComponent* ASC)
{
#if !UE_SERVER
	if (ASC)
	{
		FGameplayEffectQuery Query;
		Query.CustomMatchDelegate.BindLambda([&](const FActiveGameplayEffect& CurEffect)
		{
			return true;
		});
		ASC->RemoveActiveEffects(Query);
	}
#endif
}

void UCustomAbilitySystemBlueprintLibrary::ClearAllAbilities_ClientCallOnly(UAbilitySystemComponent* ASC)
{
#if !UE_SERVER
	if (ASC)
	{
		ASC->ClearAllAbilities();
	}
#endif
}

void UCustomAbilitySystemBlueprintLibrary::InitAttribute_ClientCallOnly(UCombatAttributeSet* InTargetAttributeSet, TMap<FString,float> InAttributes)
{
#if !UE_SERVER
	if(InTargetAttributeSet)
	{
		for(auto It : InAttributes)
		{
			FGameplayAttributeData* ValueField = nullptr;
			if (FStructProperty* TargetField = FindFProperty<FStructProperty>(UCombatAttributeSet::StaticClass(), *It.Key))
			{
				ValueField = TargetField->ContainerPtrToValuePtr<FGameplayAttributeData>(InTargetAttributeSet);
			}

			if (ValueField)
			{
				ValueField->SetBaseValue(It.Value);
				ValueField->SetCurrentValue(It.Value);
			}
		}
	}
#endif
}

void UCustomAbilitySystemBlueprintLibrary::AddAttributeSet_ClientCallOnly(UAbilitySystemComponent* ASC, UAttributeSet* InAttributeSet)
{
#if !UE_SERVER
	if (InAttributeSet)
	{
		ASC->AddAttributeSetSubobject(InAttributeSet);
	}
#endif
}
