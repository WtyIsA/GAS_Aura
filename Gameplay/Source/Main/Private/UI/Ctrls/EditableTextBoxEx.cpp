// Fill out your copyright notice in the Description page of Project Settings.


#include "EditableTextBoxEx.h"

#include "SEditableTextBoxEx.h"

UEditableTextBoxEx::UEditableTextBoxEx(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

TSharedRef<SWidget> UEditableTextBoxEx::RebuildWidget()
{
	MyEditableTextBlock = SNew(SEditableTextBoxEx)
        .Style(&WidgetStyle)
        .MinDesiredWidth(MinimumDesiredWidth)
        .IsCaretMovedWhenGainFocus(IsCaretMovedWhenGainFocus)
        .SelectAllTextWhenFocused(SelectAllTextWhenFocused)
        .RevertTextOnEscape(RevertTextOnEscape)
        .ClearKeyboardFocusOnCommit(ClearKeyboardFocusOnCommit)
        .SelectAllTextOnCommit(SelectAllTextOnCommit)
        .AllowContextMenu(AllowContextMenu)
        .OnTextChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnTextChanged))
        .OnTextCommitted(BIND_UOBJECT_DELEGATE(FOnTextCommitted, HandleOnTextCommitted))
		.OnKeyDownHandler(BIND_UOBJECT_DELEGATE(FOnKeyDown, HanderOnKeyDown))
        .VirtualKeyboardType(EVirtualKeyboardType::AsKeyboardType(KeyboardType.GetValue()))
        .VirtualKeyboardOptions(VirtualKeyboardOptions)
        .VirtualKeyboardTrigger(VirtualKeyboardTrigger)
        .VirtualKeyboardDismissAction(VirtualKeyboardDismissAction)
        .Justification(Justification);

	SEditableTextBoxEx* pEx = (SEditableTextBoxEx*)MyEditableTextBlock.Get();
	pEx->SetOnKeyUpHandler(BIND_UOBJECT_DELEGATE(FOnKeyDown, HanderOnKeyUp));
	return MyEditableTextBlock.ToSharedRef();	
}

FReply UEditableTextBoxEx::HanderOnKeyDown(const FGeometry& fg, const FKeyEvent& keyEvent)
{
	FString keyName = keyEvent.GetKey().ToString();
	if(OnKeyDown.IsBound())
	{
		for(auto temp : _hookKeyDownNames)
		{
			if (temp.Equals(keyName, ESearchCase::IgnoreCase))
			{
				FModifierKeysState keysState = FSlateApplicationBase::Get().GetPlatformApplication()->GetModifierKeys();
				if( OnKeyDown.Execute(temp, keysState.IsShiftDown(), keysState.IsControlDown(), keysState.IsAltDown(), keysState.IsCommandDown()))
					return  FReply::Handled();
				else
					return  FReply::Unhandled();
			}
		}
	}
	return FReply::Unhandled();
}

bool UEditableTextBoxEx::AddHookKeyDownName(FString keyName)
{
	for(auto temp : _hookKeyDownNames)
	{
		if (temp.Equals(keyName, ESearchCase::IgnoreCase))
		{
			return false;
		}
	}
	_hookKeyDownNames.Add(keyName);
	return  false;
}

bool UEditableTextBoxEx::RemoveHookKeyDownName(FString keyName)
{
	for(int i = 0; i < _hookKeyDownNames.Num(); i++)
	{
		auto temp = _hookKeyDownNames[i];
		if (temp.Equals(keyName, ESearchCase::IgnoreCase))
		{
			_hookKeyDownNames.RemoveAt(i);
			return true;
		}
	}
	return  false;
}

FReply UEditableTextBoxEx::HanderOnKeyUp(const FGeometry& fg, const FKeyEvent& keyEvent)
{
	FString keyName = keyEvent.GetKey().ToString();
	if(OnKeyUp.IsBound())
	{
		for(auto temp : _hookKeyUpNames)
		{
			if (temp.Equals(keyName, ESearchCase::IgnoreCase))
			{
				FModifierKeysState keysState = FSlateApplicationBase::Get().GetPlatformApplication()->GetModifierKeys();
				if( OnKeyUp.Execute(temp))
					return  FReply::Handled();
				else
					return  FReply::Unhandled();
			}
		}
	}
	return FReply::Unhandled();
}

bool UEditableTextBoxEx::AddHookKeyUpName(FString keyName)
{
	for(auto temp : _hookKeyUpNames)
	{
		if (temp.Equals(keyName, ESearchCase::IgnoreCase))
		{
			return false;
		}
	}
	_hookKeyUpNames.Add(keyName);
	return  false;
}

bool UEditableTextBoxEx::RemoveHookKeyUpName(FString keyName)
{
	for(int i = 0; i < _hookKeyUpNames.Num(); i++)
	{
		auto temp = _hookKeyUpNames[i];
		if (temp.Equals(keyName, ESearchCase::IgnoreCase))
		{
			_hookKeyUpNames.RemoveAt(i);
			return true;
		}
	}
	return  false;
}
