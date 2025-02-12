#pragma once

#include "CoreMinimal.h"
#include "Containers/Deque.h"
#include "FootPrintComponent.generated.h"

struct FHistoryStatus
{
	FVector Location;
	double TimeSeconds;
	uint8 Status;
	
	FHistoryStatus(const FVector Loc,const double Time,const uint8 Sts)
	{
		Location = Loc;
		TimeSeconds = Time;
		Status = Sts;
	}
};

UCLASS(ClassGroup=(Custom),meta=(BlueprintSpawnableComponent))
class UFootPrintComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFootPrintComponent();
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Important)
	float MaxDeltaTime = 2.0;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Important)
	float MinDeltaSize = 5.0;
	
	void AddFootPrint(FVector Location,const double TimeSeconds,const uint8 Status);

	UFUNCTION(BlueprintCallable)
	bool IsInRange(const FVector Location,const int32 Status,const float Radius,const float Height) const;

	UFUNCTION()
	int32 Size() const;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = Important)
	bool CheckStatus = true;
	
private:
	
	//TDeque<FHistoryStatus> HistoryStatuses;
};
