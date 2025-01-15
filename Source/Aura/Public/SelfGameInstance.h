// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SelfGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API USelfGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	USelfGameInstance();
	virtual void Init() override;
	virtual void OnStart() override;
	virtual void Shutdown() override;

	UFUNCTION()
	void LoadSubLevels();		
};
