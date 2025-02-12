#include "GameplayAbilityTargetData_FireProjectileInfo.h"

bool FGameplayAbilityTargetData_FireProjectileInfo::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << StartPos;
	HitResult.NetSerialize(Ar, Map, bOutSuccess);
	bOutSuccess = true;
	return true;
}
