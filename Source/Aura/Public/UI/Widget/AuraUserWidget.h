// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

	UPROPERTY(EditDefaultsOnly, Category="Lua")
	FString LuaModule;
	
	virtual FString GetModuleName_Implementation() const override
	{
		return LuaModule;
	}

	UFUNCTION()
	void ResetZOrderInViewPort(int32 zOrder);

	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
