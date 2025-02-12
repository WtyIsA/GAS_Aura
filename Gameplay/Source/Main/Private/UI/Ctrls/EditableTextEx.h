// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableText.h"
#include "SlateWrapperTypesEx.h"
#include "EditableTextEx.generated.h"


/**
 * 
 */
UCLASS()
class MAIN_API UEditableTextEx : public UEditableText
{
	GENERATED_UCLASS_BODY()

	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;
	FReply OnChar(const FGeometry&, const FCharacterEvent&);
	FReply HanderOnKeyDown(const FGeometry&, const FKeyEvent&);
	FReply HanderOnKeyUp(const FGeometry&, const FKeyEvent&);
public:
	UPROPERTY(EditAnywhere,  Category=Appearance)
	EEditInputType  _InputType ;

	UPROPERTY(EditAnywhere,  Category=Appearance)
	TArray<FString>  _hookKeyDownNames ;

	DECLARE_DYNAMIC_DELEGATE_RetVal_FiveParams(bool, FOnKeyDownEvent, FString, keyName, bool, bShift,bool, bCtrl,bool, bAlt,bool, bWin);
	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserWidgetMouseEvent, UUserWidget*, widget);
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
