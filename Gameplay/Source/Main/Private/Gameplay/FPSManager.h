// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSManager.generated.h"
UCLASS()
class MAIN_API UFPSManager : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Init(uint8 optimizeFps);
	UFUNCTION(BlueprintCallable)
	void SetTargetMaxFps(uint8 targetFps);
	UFUNCTION(BlueprintCallable)
	void SetEnable(bool bEnable = true);
	UPROPERTY()
	float CheckTime =1;
	void Tick();
	static UFPSManager* Get(UGameInstance* pGameIns = NULL);
	static  void Free();

private:
	static UFPSManager* handle;
	static UGameInstance* gameInsHandle;
	uint8 targetMaxFps;
	uint8 OptimizeFps;
	uint8 curTargetFps;
	FDelegateHandle OnSlateUserInteractionHandle;
	FDelegateHandle OnBeginFrameHandle;
	double lastInputTime = 0;
	void SetCVarMaxFps(uint8 targetFps);
	void OnSlateUserInteraction(double CurrSlateInteractionTime);
};
