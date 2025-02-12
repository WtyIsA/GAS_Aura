// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "UnLuaInterface.h"
#include "CustomGameplayCueNotify_Static.generated.h"

/**
 * 
 */
UCLASS()
class MAIN_API UCustomGameplayCueNotify_Static : public UGameplayCueNotify_Static, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	virtual void PostCDOContruct() override;
	virtual FString GetModuleName_Implementation() const override;
};
