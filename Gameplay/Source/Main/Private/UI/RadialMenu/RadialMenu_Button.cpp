// Copyright 2020 Igor Lekic | All Rights Reserved | https://igorlekic.com/

#include "RadialMenu_Button.h"

#include "Materials/MaterialInstanceDynamic.h"

bool URadialMenu_Button::Initialize()
{
	_fAangleMin = 0;
	_fAangleMAx = 0;
	if (!Super::Initialize())
	{
		return false;
	}
	CurrentButtonState = EButtonState::Disabled;
	SetPercent(0.75);
	ChangeCurrentState(EButtonState::Normal);
	
	return true;
}

//Change look of this button to new state
void URadialMenu_Button::ChangeCurrentState(EButtonState NewState)
{
	//If image of the button exist
	if (ButtonLook)
	{
		
		if(CurrentButtonState == NewState)
			return;
		//Change current state variable to new state.
		CurrentButtonState = NewState;
		//Switch look in regard to the new state.
		switch (NewState)
		{
		case EButtonState::Active:
			ButtonLook->SetBrush(Active);
			break;
		case EButtonState::Normal:
			ButtonLook->SetBrush(Normal);
			break;
		case EButtonState::Disabled:
			ButtonLook->SetBrush(Disabled);
			break;
		case EButtonState::HoveredNormal:
			ButtonLook->SetBrush(HoveredNormal);
			break;
		case EButtonState::HoveredDisabled:
			ButtonLook->SetBrush(HoveredDisabled);
			break;
		default:
			CurrentButtonState = EButtonState::Disabled;
			ButtonLook->SetBrush(Disabled);
			break;
		}
	}

}
//Refresh button look
void URadialMenu_Button::Refresh()
{
	//If image of the button exist
	if (ButtonLook)
	{
		//Switch look in regard to the current button state.
		switch (CurrentButtonState)
		{
		case EButtonState::Active:
			ButtonLook->SetBrush(Active);
			break;
		case EButtonState::Normal:
			ButtonLook->SetBrush(Normal);
			break;
		case EButtonState::Disabled:
			ButtonLook->SetBrush(Disabled);
			break;
		case EButtonState::HoveredNormal:
			ButtonLook->SetBrush(HoveredNormal);
			break;
		case EButtonState::HoveredDisabled:
			ButtonLook->SetBrush(HoveredDisabled);
			break;
		default:
			CurrentButtonState = EButtonState::Disabled;
			ButtonLook->SetBrush(Disabled);
			break;
		}
	}
}
//Button clicked event
void URadialMenu_Button::ClickEvent()
{
	//Call for blueprint event
	OnClicked();
}

void URadialMenu_Button::SetBrushPercent(FSlateBrush& brush, float fPercent)
{
	UObject* Resource = brush.GetResourceObject();
	UMaterialInterface* Material = NULL;
	Material = Cast<UMaterialInterface>(Resource);
	if ( Material )
	{
		
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);
		if ( !DynamicMaterial )
		{
			DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
			brush.SetResourceObject(DynamicMaterial);
		}

		if(DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(TEXT("Range"), fPercent);
		}
	}
}


void URadialMenu_Button::SetPercent(float fPercent)
{
	SetBrushPercent(Normal, fPercent);
	SetBrushPercent(Active, fPercent);
	SetBrushPercent(HoveredNormal, fPercent);
}

void URadialMenu_Button::SetAngle(float angleMin, float angleMax)
{
	_fAangleMin = angleMin;
	_fAangleMAx = angleMax;
}

bool URadialMenu_Button::bInAngleInterval(float angle)
{
	if(angle < _fAangleMin || angle > _fAangleMAx)
		return  false;
	return  true;
}



void URadialMenu_Button::OnClicked_Implementation()
{

}
