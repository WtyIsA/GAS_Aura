#include "GameplayAbilityTargetData_MeleeHitInfo.h"

bool FGameplayAbilityTargetData_MeleeHitInfo::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << SocketName;
	HitResult.NetSerialize(Ar, Map, bOutSuccess);
	bOutSuccess = true;
	return true;
}
