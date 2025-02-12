#pragma once
#include "SignificanceManager.h"

class MAIN_API FCustomSignificanceInterface
{
public:
	virtual ~FCustomSignificanceInterface() = default;

	struct FSignificanceSettings
	{
		struct FSignificanceThresholds
		{
			FSignificanceThresholds(float InSignificance, float InMaxDistance): Significance(InSignificance),
				MaxDistance(InMaxDistance)
			{
			}
			float Significance;
			float MaxDistance;
		};
		TArray<FSignificanceThresholds> SignificanceThresholdsArray;
	};

	FSignificanceSettings SignificanceSettings;
	void SetSignificanceSettings(const TArray<FSignificanceSettings::FSignificanceThresholds>& SignificanceThresholdsArray);
	float GetSignificanceByDistance(float Distance);

	void SignificanceRegisterObject(UObject* Object, FName Tag,UWorld* World);
	void SignificanceUnregisterObject(UObject* Object,UWorld* World);
	
	virtual float SignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo,
	                                   const FTransform& Viewpoint) ;
	virtual void PostSignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance,
	                                      float Significance, bool bFinal) ;
};
