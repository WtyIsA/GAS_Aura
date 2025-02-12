// Author: Igor Lekic - https://igorlekic.com/

#include "RadialMultLevelMenu.h"
#include "GameFramework/PlayerController.h"
#include "RadialMenu_Button.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

bool URadialMultLevelMenu::Initialize()
{
	for(auto item : MenuParams)
	{
		FMultMenuCalcParam Param;
		Param._fClickRadiaMax = item.ClickRadiaMax * item.ClickRadiaMax * item.Scale.X * item.Scale.X;
		Param._fClickRadiaMin = item.ClickRadiaMin * item.ClickRadiaMin * item.Scale.X * item.Scale.X;					
		m_menuCaluParam.Add(Param);
	}	
	if (!Super::Initialize())
	{
		return false;
	}
	
	return true;
}

int32 URadialMultLevelMenu::GetCurrentSegment(const FVector2D& MousePos, const FVector2D& CenterPos, float scale, int32& buttonId)
{
	FVector2D ReducedVector = MousePos - CenterPos;
	ReducedVector /= scale;
	float dis = ReducedVector.X*ReducedVector.X + ReducedVector.Y*ReducedVector.Y;;
	int32 nMenuLevel = -1;
	for(int i =0; i < m_menuCaluParam.Num(); i++)
	{
		if(dis >= m_menuCaluParam[i]._fClickRadiaMin && dis <= m_menuCaluParam[i]._fClickRadiaMax)
		{
			nMenuLevel = i;
			break;
		}
	}
	if(nMenuLevel < 0)
		return  -1;
	UCanvasPanel* pButtonContents = Cast<UCanvasPanel>(GetWidgetFromName(GetMenuLevelName(nMenuLevel)));
	if(!pButtonContents || pButtonContents->GetVisibility() == ESlateVisibility::Hidden || pButtonContents->GetVisibility() == ESlateVisibility::Collapsed)
		return  -1;
	float Alpha = FMath::Atan2(ReducedVector.Y, ReducedVector.X);
	/*Conversion to Degrees and add/sub 180 to round up for 360 degrees,
	then multiply by 1 or -1 to get positive numbers in regard to clockwise or counter clockwise rotation of the menu.*/
	float AimAtAngle = FMath::RadiansToDegrees(Alpha) ;
	if(AimAtAngle < 0)
		AimAtAngle += 360;
	for(auto button: m_menuCaluParam[nMenuLevel].Buttons)
	{
		if(button.Value->bInAngleInterval(AimAtAngle))
		{
			buttonId = button.Key;
			return  nMenuLevel;
		}
	}
	return  -100;
}

int32 URadialMultLevelMenu::GetCurrentSegment(float x1,float y1, float centerX,float centerY, float scale, int32& buttonId)
{
	
	const FVector2D MousePos = FVector2D(x1, y1);
	const FVector2D CenterPos(centerX,centerY);
	return  GetCurrentSegment(MousePos, CenterPos, scale, buttonId);
}


void URadialMultLevelMenu::Destructor()
{	
	//Deactivate menu
	//Call parent destruct
	Destruct();
}

void URadialMultLevelMenu::ActivateButton(int32 menuLevel,int32 ActivateIndex)
{
	if(menuLevel < 0 || menuLevel >= m_menuCaluParam.Num())
		return;
	//If menu is active and button exist
	if (m_menuCaluParam[menuLevel].Buttons.Contains(ActivateIndex))
	{
		m_menuCaluParam[menuLevel].Buttons[ActivateIndex]->ChangeCurrentState(EButtonState::Active);

	}
}

void URadialMultLevelMenu::DisableButton(int32 menuLevel,int32 ActivateIndex)
{
	if(menuLevel < 0 || menuLevel >= m_menuCaluParam.Num())
		return;
	//If menu is active and button exist
	if (m_menuCaluParam[menuLevel].Buttons.Contains(ActivateIndex))
	{
		//Disable that button
		m_menuCaluParam[menuLevel].Buttons[ActivateIndex]->ChangeCurrentState(EButtonState::Disabled);
	}
}

void URadialMultLevelMenu::DeactivateButtons(int32 menuLevel)
{
	if(menuLevel < 0 || menuLevel >= m_menuCaluParam.Num())
		return;
	//Loop all buttons
	for (auto& _Button : m_menuCaluParam[menuLevel].Buttons)
	{
		//If button was previously active and object is not this button, then deactivate it
		if (_Button.Value->CurrentButtonState == EButtonState::Active)
		{
			//Change state to normal
			_Button.Value->ChangeCurrentState(EButtonState::Normal);
		}
	}
}


void URadialMultLevelMenu::HoverOverEvent()
{
	OnHoverOver();
}



void URadialMultLevelMenu::OnHoverOver_Implementation()
{
}



void URadialMultLevelMenu::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	for(int i = 0; i < m_menuCaluParam.Num(); i++)
	{
		for (auto& _Button : m_menuCaluParam[i].Buttons)
		{
			//Switch all from hovered to NOT hovered state
			switch (_Button.Value->CurrentButtonState)
			{
			case EButtonState::HoveredNormal:
				_Button.Value->ChangeCurrentState(EButtonState::Normal);
				break;
			case EButtonState::HoveredDisabled:
				_Button.Value->ChangeCurrentState(EButtonState::Disabled);
				break;
			}
		}
	}
	_nCurrentMenuLevel = -1;
	if(_tipText.Len() != 0)
	{
		_tipText.Empty();
		SetToolTipText(FText::FromString(_tipText));
	}
	Super::NativeOnMouseLeave(InMouseEvent);
	OnMenuLeave.Broadcast(0,0);
}

FReply URadialMultLevelMenu::NativeOnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FVector2D screenPos = MouseEvent.GetScreenSpacePosition();
	FVector2D menuPos = MyGeometry.GetAbsolutePosition() + MyGeometry.GetAbsoluteSize()/2;
	int32 buttonId = -1;
	int32 _tempMenuLevel = GetCurrentSegment(screenPos, menuPos, MyGeometry.Scale,buttonId);
	
	if(_tempMenuLevel < 0)
	{
		_nCurrentMenuLevel = _tempMenuLevel;
		for(int i = 0; i < m_menuCaluParam.Num(); i++)
		{
			m_menuCaluParam[i]._nCurrentSegmentID = -1;
			for (auto& _Button : m_menuCaluParam[i].Buttons)
			{
				//Switch all from hovered to NOT hovered state
				switch (_Button.Value->CurrentButtonState)
				{
				case EButtonState::HoveredNormal:
					_Button.Value->ChangeCurrentState(EButtonState::Normal);
					break;
				case EButtonState::HoveredDisabled:
					_Button.Value->ChangeCurrentState(EButtonState::Disabled);
					break;
				}
			}
		}
		if(_tempMenuLevel != -100)
			NativeOnMouseLeave(MouseEvent);
		return FReply::Handled();
	}
	
	FReply onReply = Super::NativeOnMouseMove(MyGeometry, MouseEvent);
	if (_nCurrentMenuLevel != _tempMenuLevel || buttonId != m_menuCaluParam[_tempMenuLevel]._nCurrentSegmentID)
	{
		_nCurrentMenuLevel = _tempMenuLevel;
		//If there is change, update CurrentSegmentID to new segment ID
		m_menuCaluParam[_tempMenuLevel]._nCurrentSegmentID = buttonId;
		HoverOverEvent();
		//Loop all buttons
		for(int i = 0; i < m_menuCaluParam.Num(); i++)
		{
			for (auto& _Button : m_menuCaluParam[i].Buttons)
			{
				//Switch all from hovered to NOT hovered state
				switch (_Button.Value->CurrentButtonState)
				{
				case EButtonState::HoveredNormal:
					_Button.Value->ChangeCurrentState(EButtonState::Normal);
					break;
				case EButtonState::HoveredDisabled:
					_Button.Value->ChangeCurrentState(EButtonState::Disabled);
					break;
				}
			}
		}
		//Switch current button in segment to hovered
		auto tempButton = m_menuCaluParam[_tempMenuLevel].Buttons[buttonId];
		switch (tempButton->CurrentButtonState)
		{
		case EButtonState::Disabled:
			tempButton->ChangeCurrentState(EButtonState::HoveredDisabled);
			break;
		case EButtonState::Normal:
			tempButton->ChangeCurrentState(EButtonState::HoveredNormal);
			break;
		}
		if(_tipText != tempButton->_tipText)
		{
			_tipText = tempButton->_tipText;
			SetToolTipText(FText::FromString(_tipText));
		}
		OnMenuHover.Broadcast(_nCurrentMenuLevel, tempButton->ButtonPosition);
		return FReply::Handled();
	}
	return  onReply;
}

FReply URadialMultLevelMenu::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(_nCurrentMenuLevel < 0 || _nCurrentMenuLevel > m_menuCaluParam.Num() || m_menuCaluParam[_nCurrentMenuLevel]._nCurrentSegmentID < 0)
		return  FReply::Unhandled();
	FReply onReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
		URadialMenu_Button* pDownButton = m_menuCaluParam[_nCurrentMenuLevel].Buttons[m_menuCaluParam[_nCurrentMenuLevel]._nCurrentSegmentID];
		if(pDownButton->CurrentButtonState == EButtonState::Active)
		{
			return FReply::Handled();
		}
		if(pDownButton->CurrentButtonState == EButtonState::HoveredNormal)
		{
			for(int i = 0; i < m_menuCaluParam.Num(); i++)
			{
				for (auto& _Button : m_menuCaluParam[i].Buttons)
				{
					//Switch all from hovered to NOT hovered state
					switch (_Button.Value->CurrentButtonState)
					{
					case EButtonState::Active:
						_Button.Value->ChangeCurrentState(EButtonState::Normal);
						break;
					default:
						break;
					}
				}
			}
			//pDownButton->ChangeCurrentState(EButtonState::Active);
			OnClicked.Broadcast(_nCurrentMenuLevel,pDownButton->ButtonPosition);
		}
		return FReply::Handled();//.CaptureMouse(TakeWidget());
	
	return onReply;
}


void URadialMultLevelMenu::SetButtonCount(int32 menuLevel, int32 nCount)
{
	if(menuLevel < 0 || menuLevel >= m_menuCaluParam.Num())
		return;
	if(MenuParams[menuLevel].ButtonClass == nullptr)
	{
		return;
	}
	if(nCount <= 0)
	{
		return;
	}
	
	UCanvasPanel* pButtonContents = Cast<UCanvasPanel>(GetWidgetFromName(GetMenuLevelName(menuLevel)));
	if(pButtonContents)
	{
		pButtonContents->ClearChildren();
		m_menuCaluParam[menuLevel]._nPerRadia = 360.0f / nCount;
		m_menuCaluParam[menuLevel]._nPercent = 1 - (m_menuCaluParam[menuLevel]._nPerRadia - MenuParams[menuLevel].Interval)/360.0f;
		m_menuCaluParam[menuLevel].Buttons.Empty();

		float angle = 360.0 - m_menuCaluParam[menuLevel]._nPerRadia * 0.5;
		angle = FMath::DegreesToRadians(angle);
		m_menuCaluParam[menuLevel]._iconPos.X = MenuParams[menuLevel].IconRadiaLen * FMath::Cos(angle) - MenuParams[menuLevel].iconSize.X/2 ;
		m_menuCaluParam[menuLevel]._iconPos.Y = MenuParams[menuLevel].IconRadiaLen * FMath::Sin(angle) - MenuParams[menuLevel].iconSize.Y/2;
	}
}

FName URadialMultLevelMenu::GetMenuLevelName(int32 nLevel)
{
	FString strText = FString::Printf(TEXT("m_buttons_%d"), nLevel);
	return  FName(strText);
}

UObject* URadialMultLevelMenu::CreateOneButton(int32 nMenuLevel,int32 index, int32 nButtonId, FString tip)
{
	if(nMenuLevel < 0 || nMenuLevel >= m_menuCaluParam.Num())
		return nullptr;
	if(MenuParams[nMenuLevel].ButtonClass == nullptr)
	{
		return nullptr;
	}
	if(m_menuCaluParam[nMenuLevel]._nPerRadia <= 0)
	{
		return nullptr;
	}
	UCanvasPanel* pButtonContents = Cast<UCanvasPanel>(GetWidgetFromName(GetMenuLevelName(nMenuLevel)));
	if(pButtonContents)
	{
		URadialMenu_Button* pButton = CreateWidget<URadialMenu_Button>(this, MenuParams[nMenuLevel].ButtonClass);
		if(pButton)
		{
			pButtonContents->AddChild(pButton);
			pButton->ButtonPosition = nButtonId;
			float angle = m_menuCaluParam[nMenuLevel]._nPerRadia*index;
			if(pButton->ButtonIcon != nullptr)
			{
				pButton->ButtonIcon->SetRenderTransformAngle(0-angle);
                UCanvasPanelSlot* pIconSlot = (UCanvasPanelSlot*)pButton->ButtonIcon->Slot;
                pIconSlot->SetSize(MenuParams[nMenuLevel].iconSize);
                pIconSlot->SetPosition(m_menuCaluParam[nMenuLevel]._iconPos);
			}
			
			
			UCanvasPanelSlot* pButtonSlot = (UCanvasPanelSlot*)pButton->Slot;
			pButtonSlot->SetPosition(FVector2D(-MenuParams[nMenuLevel].btnSize.X / 2, -MenuParams[nMenuLevel].btnSize.Y / 2));
			pButtonSlot->SetAnchors(FAnchors(0.5));
			pButtonSlot->SetSize(MenuParams[nMenuLevel].btnSize);

			pButton->SetRenderTransformAngle(angle);
			pButton->SetRenderScale(MenuParams[nMenuLevel].Scale);
			pButton->SetAngle(m_menuCaluParam[nMenuLevel]._nPerRadia*(index - 1), m_menuCaluParam[nMenuLevel]._nPerRadia*index - MenuParams[nMenuLevel].Interval);
			pButton->SetPercent(m_menuCaluParam[nMenuLevel]._nPercent);
			//pButton->SetToolTipText(FText::FromString(tip));
			pButton->_tipText = tip;
			m_menuCaluParam[nMenuLevel].Buttons.Add(index-1, pButton);

			return  pButton;
		}
	}

	return nullptr;
}
