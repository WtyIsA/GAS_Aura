// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableTextBox.h"
#include "EditableTextBoxEx.generated.h"


/**
 * 
 */
UCLASS(meta=(DisplayName="Text Box Ex"))
class MAIN_API UEditableTextBoxEx : public UEditableTextBox
{
	GENERATED_UCLASS_BODY()

	virtual TSharedRef<SWidget> RebuildWidget() override;
	FReply HanderOnKeyDown(const FGeometry&, const FKeyEvent&);
	FReply HanderOnKeyUp(const FGeometry&, const FKeyEvent&);
public:
	
	UPROPERTY(EditAnywhere,  Category=Appearance)
	TArray<FString>  _hookKeyDownNames ;

	DECLARE_DYNAMIC_DELEGATE_RetVal_FiveParams(bool, FOnKeyDownEvent, FString, keyName, bool, bShift,bool, bCtrl,bool, bAlt,bool, bWin);
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnKeyDownEvent OnKeyDown;

	UFUNCTION()
	bool AddHookKeyDownName(FString keyName);
	UFUNCTION()
    bool RemoveHookKeyDownName(FString keyName);

	UPROPERTY(EditAnywhere,  Category=Appearance)
	TArray<FString>  _hookKeyUpNames ;

	DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FOnKeyUpEvent, FString, keyName);
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnKeyUpEvent OnKeyUp;

	UFUNCTION()
    bool AddHookKeyUpName(FString keyName);
	UFUNCTION()
    bool RemoveHookKeyUpName(FString keyName);
};
