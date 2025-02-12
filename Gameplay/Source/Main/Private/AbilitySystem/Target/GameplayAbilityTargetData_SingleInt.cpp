#include "GameplayAbilityTargetData_SingleInt.h"

bool FGameplayAbilityTargetData_SingleInt::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << IntValue;
	bOutSuccess = true;
	return true;
}
