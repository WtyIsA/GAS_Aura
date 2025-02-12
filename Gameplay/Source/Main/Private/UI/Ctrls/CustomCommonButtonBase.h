// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "Components/CheckBox.h"
#include "CommonTextBlock.h"
#include "CustomCommonButtonBase.generated.h"

class UCommonTextBlock;

UCLASS(Blueprintable, ClassGroup = UI, meta = (Category = "Common UI", DisableNativeTick))
class MAIN_API UCustomCommonButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnDoubleClicked() override;
	virtual void NativeOnClicked() override;
	virtual void HandleButtonPressed() override;
	virtual void HandleButtonReleased() override;

	void CheckStateChange(bool bChecked);
	
public:
	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable, Category="Widget")
	void SetCheckedState(ECheckBoxState InCheckedState, bool bPlaySound = true);
	
	UFUNCTION(BlueprintCallable, Category="Widget")
	ECheckBoxState GetCheckedState() const;

	UFUNCTION(BlueprintCallable, Category="Widget")
	bool IsChecked() const;
	
	UPROPERTY(BlueprintAssignable, Category = "Events", meta = (AllowPrivateAccess = true))
	FCommonButtonBaseClicked OnClicked;
	
	UPROPERTY(BlueprintAssignable, Category = "Events", meta = (AllowPrivateAccess = true))
	FCommonButtonBaseClicked HandleDoubleClicked;

	UPROPERTY(BlueprintAssignable, Category = "Events", meta = (AllowPrivateAccess = true))
	FCommonButtonBaseClicked OnPressed;

	UPROPERTY(BlueprintAssignable, Category = "Events", meta = (AllowPrivateAccess = true))
	FCommonButtonBaseClicked OnReleased;

	UPROPERTY(BlueprintAssignable, Category = "Events", meta = (AllowPrivateAccess = true))
	FOnCheckBoxComponentStateChanged OnCheckStateChanged;
protected:
	virtual void NativeOnCurrentTextStyleChanged() override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, OptionalWidget = true, AllowPrivateAccess = true), Category = "Text Block")
	UCommonTextBlock* Custom_Text;
};
