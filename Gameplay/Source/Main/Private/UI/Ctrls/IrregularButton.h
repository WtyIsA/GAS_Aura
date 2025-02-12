// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "IrregularButton.generated.h"

/**
 * 
 */
UCLASS()
class  UIrregularButton : public UButton
{
	GENERATED_UCLASS_BODY()
	public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AdvancedHitTest") 
	UTexture2D* AdvancedHitTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AdvancedHitTest", meta = (ClampMin = "0.0", ClampMax = "255.0", UIMin = "0.0", UIMax = "255.0"))
	int AdvancedHitAlpha;

	UFUNCTION(BlueprintCallable, Category = "AdvancedHitTest") 
        void SetAdvancedHitTexture(UTexture2D* InTexture);
	UFUNCTION(BlueprintCallable, Category = "AdvancedHitTest") 
        void SetAdvancedHitAlpha(int InAlpha);


	virtual void SynchronizeProperties() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
};
