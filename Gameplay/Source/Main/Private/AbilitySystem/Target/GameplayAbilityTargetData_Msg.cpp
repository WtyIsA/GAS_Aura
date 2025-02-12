#include "GameplayAbilityTargetData_Msg.h"

bool FGameplayAbilityTargetData_Msg::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << Data;
	HitResult.NetSerialize(Ar, Map, bOutSuccess);
	SafeNetSerializeTArray_Default<31>(Ar, TargetActorArray);
	bOutSuccess = true;
	return true;
}
