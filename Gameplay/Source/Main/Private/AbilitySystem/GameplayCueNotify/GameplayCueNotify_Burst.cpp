// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayCueNotify_Burst.h"

//////////////////////////////////////////////////////////////////////////
// UGameplayCueNotify_Burst
//////////////////////////////////////////////////////////////////////////
UGameplayCueNotify_Burst::UGameplayCueNotify_Burst()
{
}

bool UGameplayCueNotify_Burst::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	UWorld* World = (Target ? Target->GetWorld() : GetWorld());

	FGameplayCueNotify_SpawnContext SpawnContext(World, Target, Parameters);
	SpawnContext.SetDefaultSpawnCondition(&DefaultSpawnCondition);
	SpawnContext.SetDefaultPlacementInfo(&DefaultPlacementInfo);

	if (DefaultSpawnCondition.ShouldSpawn(SpawnContext))
	{
		FGameplayCueNotify_SpawnResult SpawnResult;

		BurstEffects.ExecuteEffects(SpawnContext, SpawnResult);

		OnBurst(Target, Parameters, SpawnResult);
	}

	return false;
}

