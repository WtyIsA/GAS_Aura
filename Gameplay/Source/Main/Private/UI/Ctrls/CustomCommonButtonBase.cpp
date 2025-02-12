// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCommonButtonBase.h"

bool UCustomCommonButtonBase::Initialize()
{
	const bool bInitializedThisCall = Super::Initialize();

	if (bInitializedThisCall)
	{
		OnIsSelectedChanged().AddUObject(this,&UCustomCommonButtonBase::CheckStateChange);
	}

	return bInitializedThisCall;
}

void UCustomCommonButtonBase::NativeOnDoubleClicked()
{
	Super::NativeOnDoubleClicked();
	if (HandleDoubleClicked.IsBound())
	{
		HandleDoubleClicked.Broadcast(this);
	}
}

void UCustomCommonButtonBase::NativeOnClicked()
{
	Super::NativeOnClicked();
	if (OnClicked.IsBound())
	{
		OnClicked.Broadcast(this);
	}
}

void UCustomCommonButtonBase::HandleButtonPressed()
{
	if (OnPressed.IsBound())
	{
		OnPressed.Broadcast(this);
	}
}

void UCustomCommonButtonBase::HandleButtonReleased()
{
	if (OnReleased.IsBound())
	{
		OnReleased.Broadcast(this);
	}
}

void UCustomCommonButtonBase::CheckStateChange(bool bChecked)
{
	if (OnCheckStateChanged.IsBound())
	{
		OnCheckStateChanged.Broadcast(bChecked);
	}
}

void UCustomCommonButtonBase::SetCheckedState(ECheckBoxState InCheckedState, bool bPlaySound)
{
	SetIsSelected(InCheckedState == ECheckBoxState::Checked ? true : false, bPlaySound);
}

ECheckBoxState UCustomCommonButtonBase::GetCheckedState() const
{
	if (GetSelected())
	{
		return ECheckBoxState::Checked;
	}
	return ECheckBoxState::Unchecked;
}

bool UCustomCommonButtonBase::IsChecked() const
{
	return GetSelected();
}

void UCustomCommonButtonBase::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();

	if (Custom_Text)
	{
		Custom_Text->SetStyle(GetCurrentTextStyleClass());
	}
}
