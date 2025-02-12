// Fill out your copyright notice in the Description page of Project Settings.


#include "EditableTextEx.h"
#include "SEditableTextEx.h"

UEditableTextEx::UEditableTextEx(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	_InputType = EEditInputType::Normal;
}

TSharedRef<SWidget> UEditableTextEx::RebuildWidget()
{
	//Super::RebuildWidget();
	MyEditableText = SNew( SEditableTextEx )
        .Style( &WidgetStyle )
        .MinDesiredWidth( MinimumDesiredWidth )
        .IsCaretMovedWhenGainFocus( IsCaretMovedWhenGainFocus )
        .SelectAllTextWhenFocused( SelectAllTextWhenFocused )
        .RevertTextOnEscape( RevertTextOnEscape )
        .ClearKeyboardFocusOnCommit( ClearKeyboardFocusOnCommit )
        .SelectAllTextOnCommit( SelectAllTextOnCommit )
        .OnTextChanged( BIND_UOBJECT_DELEGATE( FOnTextChanged, HandleOnTextChanged ) )
        .OnTextCommitted( BIND_UOBJECT_DELEGATE( FOnTextCommitted, HandleOnTextCommitted ) )
        .VirtualKeyboardType( EVirtualKeyboardType::AsKeyboardType( KeyboardType.GetValue() ) )
        .VirtualKeyboardOptions(VirtualKeyboardOptions)
        .VirtualKeyboardTrigger(VirtualKeyboardTrigger)
        .VirtualKeyboardDismissAction(VirtualKeyboardDismissAction)
        .Justification( Justification );
		
	MyEditableText->SetOnKeyDownHandler(BIND_UOBJECT_DELEGATE(FOnKeyDown, HanderOnKeyDown));
	SEditableTextEx* pEx = (SEditableTextEx*)MyEditableText.Get();
	pEx->SetOnKeyUpHandler(BIND_UOBJECT_DELEGATE(FOnKeyDown, HanderOnKeyUp));
	return MyEditableText.ToSharedRef();
}

void UEditableTextEx::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	SEditableTextEx* pEdit = (SEditableTextEx*)(MyEditableText.Get());
	if(pEdit)
	{
		pEdit->_InputType = _InputType;
		if(pEdit->CheckInputText(GetText()))
			pEdit->_oldText = GetText();
		else
		{
			if(!pEdit->CheckInputText(pEdit->_oldText))
			{
				pEdit->_oldText = FText::FromString(TEXT(""));
			}
			SetText(pEdit->_oldText);
			
		}
		
	}
	MyEditableText->SetOnKeyCharHandler(FOnKeyChar::CreateUObject(this, &UEditableTextEx::OnChar));
}

FReply UEditableTextEx::OnChar(const FGeometry& fg, const FCharacterEvent& fCharEvent)
{
	TCHAR c = fCharEvent.GetCharacter();
	if(c == 8)
		return  FReply::Unhandled();

	//FKey const Key = FInputKeyManager::Get().GetKeyFromCodes( c, c );
	
	if(_InputType == EEditInputType::Number)
	{
		/*
		 //是否支持负数和小数，暂时不需要
		if(c == TEXT('-'))
     	{
			const FString& str = FTextInspector::GetDisplayString( GetText());
     		if(str.IsEmpty() || (str[0]<= TEXT('9') && str[0] >= TEXT('0')) )
     			return FReply::Unhandled();
     	}
		if(c == TEXT('.'))
		{
			const FString& str = FTextInspector::GetDisplayString( GetText());
			int nPos = 0;
			if(!str.FindChar(c,nPos))
			{
				return  FReply::Unhandled();
			}
		}
		*/
		if( c <= TEXT('9') && c >= TEXT('0'))
			return FReply::Unhandled();
		else
			return  FReply::Handled();
	}
	else if(_InputType == EEditInputType::AlphaAndNumber)
	{
		if(isalnum(c))
			return  FReply::Unhandled();
		else
			return  FReply::Handled();
	}
	return  FReply::Unhandled();
}

FReply UEditableTextEx::HanderOnKeyDown(const FGeometry& fg, const FKeyEvent& keyEvent)
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

bool UEditableTextEx::AddHookKeyDownName(FString keyName)
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

bool UEditableTextEx::RemoveHookKeyDownName(FString keyName)
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

FReply UEditableTextEx::HanderOnKeyUp(const FGeometry& fg, const FKeyEvent& keyEvent)
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

bool UEditableTextEx::AddHookKeyUpName(FString keyName)
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

bool UEditableTextEx::RemoveHookKeyUpName(FString keyName)
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
