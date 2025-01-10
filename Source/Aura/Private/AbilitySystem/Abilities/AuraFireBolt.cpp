// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 Damage = DamageTypes[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);

	if (Level == 1)
	{
		return FString::Printf(TEXT(
			"<Title>Fire Bolt</>\n\n<Default>Launches a bolt of fire,exploding on impact and dealing:</><Damage>%d</><Default>"
			"fire damage with a chance to burn</>\n\n<Small>Level%d</>"), Damage, Level);
	}
	else
	{
		return FString::Printf(TEXT("<Title>Fire Bolt</>\n\n<Default>Launches %d bolt of fire,exploding on impact and dealing:</><Damage>%d</><Default>fire damage with a chance to burn</>\n\n<Small>Level%d</>"), FMath::Min(Level, NumProjectiles), Damage, Level);
	}
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 Damage = DamageTypes[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level);

	return FString::Printf(TEXT("<Title>Next Level</>\n\n<Default>Launches %d bolt of fire,exploding on impact and dealing:</><Damage>%d</><Default>fire damage with a chance to burn</>\n\n<Small>Level%d</>"), FMath::Min(Level, NumProjectiles), Damage, Level);
}
