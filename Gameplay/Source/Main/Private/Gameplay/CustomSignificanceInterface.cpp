#include "Gameplay\CustomSignificanceInterface.h"

void FCustomSignificanceInterface::SetSignificanceSettings(
	const TArray<FSignificanceSettings::FSignificanceThresholds>& SignificanceThresholdsArray)
{
	SignificanceSettings.SignificanceThresholdsArray = SignificanceThresholdsArray;
}

float FCustomSignificanceInterface::GetSignificanceByDistance(float Distance)
{
	const int32 NumThresholds = SignificanceSettings.SignificanceThresholdsArray.Num();
	if (Distance >= SignificanceSettings.SignificanceThresholdsArray[NumThresholds-1].MaxDistance)
	{
		return SignificanceSettings.SignificanceThresholdsArray[NumThresholds-1].Significance;
	}
	for (const auto Setting : SignificanceSettings.SignificanceThresholdsArray)
	{
		if (Distance<=Setting.MaxDistance)
		{
			return Setting.Significance;
		}
	}
	return  0.f;
}

void FCustomSignificanceInterface::SignificanceRegisterObject(UObject* Object, FName Tag, UWorld* World)
{
	USignificanceManager* SignificanceManager = FSignificanceManagerModule::Get(World);
	if (SignificanceManager)
	{
		auto significance = [&](USignificanceManager::FManagedObjectInfo* ObjectInfo,
		                        const FTransform& Viewpoint)-> float
		{
			return SignificanceFunction(ObjectInfo, Viewpoint);
		};
		auto PostSignificance = [&](USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance,
		                            float Significance, bool bFinal)
		{
			PostSignificanceFunction(ObjectInfo, OldSignificance, Significance, bFinal);
		};
		SignificanceManager->RegisterObject(Object, Tag, significance,
		                                    USignificanceManager::EPostSignificanceType::Concurrent, PostSignificance);
	}
}

void FCustomSignificanceInterface::SignificanceUnregisterObject(UObject* Object, UWorld* World)
{
	if (World)
	{
		USignificanceManager* SignificanceManager = FSignificanceManagerModule::Get(World);
		if (SignificanceManager)
		{
			SignificanceManager->UnregisterObject(Object);
		}
	}
}

float FCustomSignificanceInterface::SignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo,
                                                         const FTransform& Viewpoint)
{
	return 0;
}

void FCustomSignificanceInterface::PostSignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo,
                                                            float OldSignificance, float Significance, bool bFinal)
{
}
