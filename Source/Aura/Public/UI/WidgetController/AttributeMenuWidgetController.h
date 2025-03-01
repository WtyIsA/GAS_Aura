// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

class UAttributeInfo;
struct FAuraAttributeInfo;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAuraAttributeInfo&, Info);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributePointsChangedSignature, int32, AttributePoints);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attribute")
	FAttributeInfoSignature AttributeInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attribute")
	FOnPlayerStatChangedSignature AttributePointsChangedDelegate;


	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;

	UFUNCTION(BlueprintCallable)
	void UpgradeAttribute(const FGameplayTag& AttributeTag);


private:
	void BroadcastAttributeInfo(const FGameplayTag& GameplayTag, const FGameplayAttribute& Attribute) const;
};
