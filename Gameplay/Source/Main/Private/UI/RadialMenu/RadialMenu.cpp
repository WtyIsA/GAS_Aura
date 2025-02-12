// Author: Igor Lekic - https://igorlekic.com/

#include "RadialMenu.h"
#include "GameFramework/PlayerController.h"
#include "RadialMenu_Button.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TileView.h"

bool URadialMenu::Initialize()
{
	_radiaLen = FMath::Max(btnSize.X, btnSize.Y);
	_radiaLen *= _radiaLen;
	_clickRadiaMax = clickRadiaMax*clickRadiaMax;
	_clickRadiaMin = clickRadiaMin*clickRadiaMin;
	CurrentSegmentID= -1;
	_nPerRadia = 0;
	if (!Super::Initialize())
	{
		return false;
	}
	return true;
}

float URadialMenu::GetCurrentSegment(const FVector2D& MousePos, const FVector2D& CenterPos, float scale)
{
	FVector2D ReducedVector = MousePos - CenterPos;
	ReducedVector /= scale;
	float dis = ReducedVector.X*ReducedVector.X + ReducedVector.Y*ReducedVector.Y;
	if(dis > _clickRadiaMax || dis < _clickRadiaMin )
		return  -1;
	float Alpha = FMath::Atan2(ReducedVector.Y, ReducedVector.X);
	/*Conversion to Degrees and add/sub 180 to round up for 360 degrees,
	then multiply by 1 or -1 to get positive numbers in regard to clockwise or counter clockwise rotation of the menu.*/
	float AimAtAngle = FMath::RadiansToDegrees(Alpha) ;
	if(AimAtAngle < 0)
		AimAtAngle += 360;
	return  GetCurrentSegment(AimAtAngle);
}

float URadialMenu::GetCurrentSegment(float x1,float y1, float centerX,float centerY, float scale)
{
	
	const FVector2D MousePos = FVector2D(x1, y1);
	const FVector2D CenterPos(centerX,centerY);
	return  GetCurrentSegment(MousePos, CenterPos, scale);
}

float URadialMenu::GetCurrentSegment(float angle)
{
	for (auto buffon : Buttons)
	{
		if(buffon.Value->bInAngleInterval(angle))
			return  buffon.Key;
	}
	return  -1;
}

void URadialMenu::Destructor()
{
	if (InputComponent)
	{
		//Clear any binding from this object
		InputComponent->KeyBindings.Empty();
	}
	//Deactivate menu
	//Call parent destruct
	Destruct();
}

void URadialMenu::PreselectButtonGamepad(float ThumbValue_X, float ThumbValue_Y)
{
	//Create Vector from X and Y
	FVector GamePadPosition = FVector(ThumbValue_X, ThumbValue_Y,0);
	//Check if thumbstick is moved
	if (GamePadPosition.Size()>0.00f)
	{
		//Get rotation from thumbstick vector
		FRotator Angle = GamePadPosition.Rotation();
		//Check if menu is active
		
		{
			//Get angle from thumbstick and adjust it to parameters of the menu
			float AimAtAngle = ((Angle.Yaw-90) -180 ) * ( -1 );

			//Locally get current segment ID from given angle.
			int32 _tempSegment = GetCurrentSegment(AimAtAngle);
			/*Compare if current segment is not equal to lastly updated segment(CurrentSegmentID) to avoid looping each frame if there is no change.
			Also check if button exist in Buttons container to avoid errors.
			*/
			if (_tempSegment != CurrentSegmentID && Buttons.Contains(_tempSegment))
			{
				//If there is change, update CurrentSegmentID to new segment ID
				CurrentSegmentID = _tempSegment;
				HoverOverEvent();
				//Loop all buttons
				for (auto& _Button : Buttons)
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
				//Switch current button in segment to hovered
				switch (Buttons[_tempSegment]->CurrentButtonState)
				{
				case EButtonState::Disabled:
					Buttons[_tempSegment]->ChangeCurrentState(EButtonState::HoveredDisabled);
					break;
				case EButtonState::Normal:
					Buttons[_tempSegment]->ChangeCurrentState(EButtonState::HoveredNormal);
					break;
				}
			}
		}
	}
}


void URadialMenu::ActivateButton(int32 ActivateIndex)
{
	//If menu is active and button exist
	if (Buttons.Contains(ActivateIndex))
	{
		Buttons[ActivateIndex]->ChangeCurrentState(EButtonState::Active);

	}
}

void URadialMenu::DisableButton(int32 ActivateIndex)
{
	//If menu is active and button exist
	if (Buttons.Contains(ActivateIndex))
	{
		//Disable that button
		Buttons[ActivateIndex]->ChangeCurrentState(EButtonState::Disabled);
	}
}

void URadialMenu::DeactivateButtons()
{
	//Loop all buttons
	for (auto& _Button : Buttons)
	{
		//If button was previously active and object is not this button, then deactivate it
		if (_Button.Value->CurrentButtonState == EButtonState::Active)
		{
			//Change state to normal
			_Button.Value->ChangeCurrentState(EButtonState::Normal);
		}
	}
}


void URadialMenu::HoverOverEvent()
{
	OnHoverOver();
}



void URadialMenu::OnHoverOver_Implementation()
{
}



void URadialMenu::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	CurrentSegmentID = -1;
	for (auto& _Button : Buttons)
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
	Super::NativeOnMouseLeave(InMouseEvent);
}

FReply URadialMenu::NativeOnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FVector2D screenPos = MouseEvent.GetScreenSpacePosition();
	FVector2D LocalPosition = MyGeometry.AbsoluteToLocal(screenPos);
	LocalPosition.X = floor(LocalPosition.X);
	LocalPosition.Y = floor(LocalPosition.Y);
	FVector2D menuPos = MyGeometry.GetAbsolutePosition() + MyGeometry.GetAbsoluteSize()/2;
	int32 _tempSegment = GetCurrentSegment(screenPos, menuPos, MyGeometry.Scale);
	if(_tempSegment < 0)
	{
		CurrentSegmentID = _tempSegment;
		for (auto& _Button : Buttons)
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
		NativeOnMouseLeave(MouseEvent);
		return FReply::Unhandled();
	}
	
	FReply onReply = Super::NativeOnMouseMove(MyGeometry, MouseEvent);
	if (_tempSegment != CurrentSegmentID && Buttons.Contains(_tempSegment))
	{
		//If there is change, update CurrentSegmentID to new segment ID
		CurrentSegmentID = _tempSegment;
		HoverOverEvent();
		//Loop all buttons
		for (auto& _Button : Buttons)
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
		//Switch current button in segment to hovered
		switch (Buttons[_tempSegment]->CurrentButtonState)
		{
		case EButtonState::Disabled:
			Buttons[_tempSegment]->ChangeCurrentState(EButtonState::HoveredDisabled);
			break;
		case EButtonState::Normal:
			Buttons[_tempSegment]->ChangeCurrentState(EButtonState::HoveredNormal);
			break;
		}
		return FReply::Handled();
	}
	return  onReply;
}

FReply URadialMenu::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	
	FReply onReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if(Buttons.Contains(CurrentSegmentID))
	{
		URadialMenu_Button* pDownButton = Buttons[CurrentSegmentID];
		if(pDownButton->CurrentButtonState == EButtonState::Active)
		{
			return FReply::Handled();
		}
		if(pDownButton->CurrentButtonState == EButtonState::HoveredNormal)
		{
			for (auto& _Button : Buttons)
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
			pDownButton->ChangeCurrentState(EButtonState::Active);
			OnClicked.Broadcast(pDownButton->ButtonPosition);
		}
		return FReply::Handled();//.CaptureMouse(TakeWidget());
	}
	return onReply;
}

void URadialMenu::SetButtonPath(FString path)
{
	//UClass* widget = LoadClass<UUserWidget>(nullptr, TEXT("/Game/UI/UMG_texurePacker.UMG_texurePacker_C"));
	_buttonClass = LoadClass<UUserWidget>(nullptr, *path);
	
}

void URadialMenu::SetButtonCount(int32 nCount)
{
	if(_buttonClass == nullptr)
	{
		return;
	}
	if(nCount <= 0)
	{
		return;
	}
	UCanvasPanel* pButtonContents = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("m_buttons")));
	if(pButtonContents)
	{
		pButtonContents->ClearChildren();
		_nPerRadia = 360.0f / nCount;
		_nPercent = 1 - (_nPerRadia - fInterval)/360.0f;
	}
}


void URadialMenu::CreateButtons(int32 nCount)
{
	if(_buttonClass == nullptr)
	{
		return;
	}
	if(nCount <= 0)
	{
		return;
	}
	UCanvasPanel* pButtonContents = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("m_buttons")));
	if(pButtonContents)
	{
		pButtonContents->ClearChildren();
		float nPerRadia = 360 / nCount;
		float nPercent = 1 - (nPerRadia - fInterval)/360.0f;
		for(int i = 0; i < nCount; i++)
		{
			URadialMenu_Button* pButton = CreateWidget<URadialMenu_Button>(this, _buttonClass);
			if(pButton)
			{
				pButtonContents->AddChild(pButton);
				pButton->ButtonPosition = i;
				UCanvasPanelSlot* pButtonSlot = (UCanvasPanelSlot*)pButton->Slot;
				pButtonSlot->SetPosition(FVector2D(-btnSize.X / 2, -btnSize.Y / 2));
				pButtonSlot->SetAnchors(FAnchors(0.5));
				pButtonSlot->SetSize(FVector2D(btnSize.X, btnSize.Y));
				pButton->SetRenderTransformAngle(nPerRadia*(i+1));
				pButton->SetAngle(nPerRadia*i, nPerRadia*i + nPerRadia - fInterval);
				pButton->SetPercent(nPercent);
				Buttons.Add(i, pButton);
			}
		}
	}	
}

void URadialMenu::CreateOneButton(int32 index, int32 nButtonId, FString iconPath, FString tip)
{
	if(_buttonClass == nullptr)
	{
		return;
	}
	if(_nPerRadia <= 0)
	{
		return;
	}
	UCanvasPanel* pButtonContents = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("m_buttons")));
	if(pButtonContents)
	{
		URadialMenu_Button* pButton = CreateWidget<URadialMenu_Button>(this, _buttonClass);
		if(pButton)
		{
			pButtonContents->AddChild(pButton);
			pButton->ButtonPosition = nButtonId;
			UCanvasPanelSlot* pButtonSlot = (UCanvasPanelSlot*)pButton->Slot;
			pButtonSlot->SetPosition(FVector2D(-btnSize.X / 2, -btnSize.Y / 2));
			pButtonSlot->SetAnchors(FAnchors(0.5));
			pButtonSlot->SetSize(FVector2D(btnSize.X, btnSize.Y));
			pButton->SetRenderTransformAngle(_nPerRadia*index);
			pButton->SetAngle(_nPerRadia*(index - 1), _nPerRadia*index - fInterval);
			pButton->SetPercent(_nPercent);
			pButton->SetToolTipText(FText::FromString(tip));
			Buttons.Add(index-1, pButton);
			
		}
	}
	
}
