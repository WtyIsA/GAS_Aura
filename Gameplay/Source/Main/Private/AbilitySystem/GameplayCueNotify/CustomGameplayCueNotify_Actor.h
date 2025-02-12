// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "UnLuaInterface.h"
#include "CustomGameplayCueNotify_Actor.generated.h"

/**
 * 
 */
UCLASS()
class MAIN_API ACustomGameplayCueNotify_Actor : public AGameplayCueNotify_Actor, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	virtual void PostCDOContruct() override;
	virtual FString GetModuleName_Implementation() const override;
};
