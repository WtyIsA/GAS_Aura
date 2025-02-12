

#include "CustomCalculation.h"
#include "UnLuaBase.h"

void UCustomCalculation::PostCDOContruct()
{
	UnLua::ClassBindLuaFile(this, false);
}

float UCustomCalculation::GetAttributeCaptureSpec(const FGameplayEffectSpec& Spec, const TSubclassOf<UCustomAttributeSet> AttributeSetClass, const FString& InName, const EGameplayEffectAttributeCaptureSource InType)
{
	FName AttrName = *InName;
	FGameplayEffectAttributeCaptureDefinition AttrDef(UCustomAttributeSet::GetPropertyByName(AttributeSetClass,AttrName), InType, true);
	float AttribValue = 0.0f;
	const FGameplayEffectAttributeCaptureSpec* CapturedAttribute =  Spec.CapturedRelevantAttributes.FindCaptureSpecByDefinition(AttrDef,true);
	if (CapturedAttribute)
	{
		FAggregatorEvaluateParameters EvalParams;
		
		EvalParams.SourceTags = &Spec.CapturedSourceTags.GetActorTags();
		EvalParams.TargetTags = &Spec.CapturedTargetTags.GetActorTags();
		CapturedAttribute->AttemptCalculateAttributeMagnitude(EvalParams,AttribValue);
		return AttribValue;
	}
	ABILITY_LOG(Error,TEXT("Not find captured attribute with name : %s, source from : %s"),*InName,InType == EGameplayEffectAttributeCaptureSource::Source ? TEXT("Source") : TEXT("Target"))
	return AttribValue;
}

FString UCustomCalculation::GetModuleName_Implementation() const
{
	return FString("Share.GAS.Calculations.DamageCalculation");
}

float UCustomCalculation::GetSetByCaller(const FGameplayEffectSpec& Spec, FName& InName, float InDefaultValue)
{
	return Spec.GetSetByCallerMagnitude(InName,false,InDefaultValue);
}

float UCustomCalculation::GetTagSetByCaller(const FGameplayEffectSpec& Spec, FGameplayTag& InTag, float InDefaultValue)
{
	return Spec.GetSetByCallerMagnitude(InTag,false,InDefaultValue);
}