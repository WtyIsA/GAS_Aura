// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "UnLuaInterface.h"
#include "DangerTriggerVolume.generated.h"

UCLASS(Blueprintable, BlueprintType)
class MAIN_API ADangerTriggerVolume : public ATriggerVolume,public IUnLuaInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADangerTriggerVolume();
	virtual FString GetModuleName_Implementation() const  {
		return 	TEXT("Share.StrongHold.DangerVolume");
	}
	virtual bool NeedsLoadForServer() const override { return false; }
	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);
	UFUNCTION()
	void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION(BlueprintNativeEvent)
	void Enter(AActor* OtherActor);
	UFUNCTION(BlueprintNativeEvent)
	void Exit(AActor* OtherActor);
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int MidLevel = 0;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int HighLevel = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
