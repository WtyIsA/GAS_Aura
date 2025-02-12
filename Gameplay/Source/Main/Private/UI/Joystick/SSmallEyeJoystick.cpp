#include "SSmallEyeJoystick.h"



#include "SelfJoystick.h"
#include "SmallEyeJoystick.h"
#include "Engine/LocalPlayer.h"

SSmallEyeJoyStrick::SSmallEyeJoyStrick():SSelfBaseJoystick()
{
	_inputScale = 1.0f;
	_deadZone = 0.0f;
	_bTouching = false;
}

void SSmallEyeJoyStrick::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (State == State_WaitForStart || State == State_CountingDownToStart)
	{
		CurrentOpacity = 0.f;
	}
	else
	{
		// lerp to the desired opacity based on whether the user is interacting with the joystick
		CurrentOpacity = FMath::Lerp(CurrentOpacity, GetBaseOpacity(), 3.f * InDeltaTime);
	}

	// count how many controls are active
	int32 NumActiveControls = 0;

	// figure out how much to scale the control sizes
	float ScaleFactor = 1.0f;// AllottedGeometry.Scale;
	for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
	{
		FControlData& Control = Controls[ControlIndex];

		if (Control.bNeedUpdatedCenter)
		{
			Control.ElapsedTime += InDeltaTime;
			if (Control.ElapsedTime > ActivationDelay)
			{
				Control.bNeedUpdatedCenter = false;
				CurrentOpacity = ActiveOpacity;

				if (!bPreventReCenter)
				{
					Control.VisualCenter = Control.NextCenter;
				}

				HandleTouch(ControlIndex, Control.NextCenter, AllottedGeometry.GetLocalSize());
			}
		}

		// calculate absolute positions based on geometry
		// @todo: Need to manage geometry changing!
		if (!Control.bHasBeenPositioned || ScaleFactor != PreviousScalingFactor)
		{
			const FControlInfo& ControlInfo = Control.Info;
			
			// update all the sizes
			Control.CorrectedCenter = FVector2D(ResolveRelativePosition(ControlInfo.Center.X, AllottedGeometry.GetLocalSize().X, ScaleFactor), ResolveRelativePosition(ControlInfo.Center.Y, AllottedGeometry.GetLocalSize().Y, ScaleFactor));
			Control.VisualCenter = Control.CorrectedCenter;
			Control.CorrectedVisualSize = FVector2D(ResolveRelativePosition(ControlInfo.VisualSize.X, AllottedGeometry.GetLocalSize().X, ScaleFactor), ResolveRelativePosition(ControlInfo.VisualSize.Y, AllottedGeometry.GetLocalSize().Y, ScaleFactor));
			Control.CorrectedInteractionSize = FVector2D(ResolveRelativePosition(ControlInfo.InteractionSize.X, AllottedGeometry.GetLocalSize().X, ScaleFactor), ResolveRelativePosition(ControlInfo.InteractionSize.Y, AllottedGeometry.GetLocalSize().Y, ScaleFactor));
			Control.CorrectedThumbSize = FVector2D(ResolveRelativePosition(ControlInfo.ThumbSize.X, AllottedGeometry.GetLocalSize().X, ScaleFactor), ResolveRelativePosition(ControlInfo.ThumbSize.Y, AllottedGeometry.GetLocalSize().Y, ScaleFactor));
			Control.CorrectedInputScale = ControlInfo.InputScale; // *ScaleFactor;
			Control.bHasBeenPositioned = true;
		}

		if (Control.CapturedPointerIndex >= 0 || Control.bSendOneMoreEvent)
		{
			Control.bSendOneMoreEvent = false;
			
				const FGamepadKeyNames::Type XAxis = (Control.Info.MainInputKey.IsValid()
                                              ? Control.Info.MainInputKey.GetFName()
											  : (ControlIndex == 0
											  	   ? FGamepadKeyNames::LeftAnalogX
											  	   : FGamepadKeyNames::RightAnalogX));
				const FGamepadKeyNames::Type YAxis = (Control.Info.AltInputKey.IsValid()
                                                          ? Control.Info.AltInputKey.GetFName()
                                                          : (ControlIndex == 0
																 ? FGamepadKeyNames::LeftAnalogY
																 : FGamepadKeyNames::RightAnalogY));
			FSlateApplication::Get().OnControllerAnalog(XAxis, ControlIndex, 0);
			FSlateApplication::Get().OnControllerAnalog(YAxis, ControlIndex, 0);
		}

		// is this active?
		if (Control.CapturedPointerIndex != -1)
		{
			NumActiveControls++;
		}
	}

	// we need to store the computed scale factor so we can compare it with the value computed in the following frame and, if necessary, recompute widget position
	PreviousScalingFactor = ScaleFactor;

	// STATE MACHINE!
	if (NumActiveControls > 0 || bPreventReCenter)
	{
		// any active control snaps the state to active immediately
		State = State_Active;
	}
	else
	{
		switch (State)
		{
			case State_WaitForStart:
				{
					State = State_CountingDownToStart;
					Countdown = StartupDelay;
				}
				break;
			case State_CountingDownToStart:
				// update the countdown
				Countdown -= InDeltaTime;
				if (Countdown <= 0.0f)
				{
					State = State_Inactive;
				}
				break;
			case State_Active:
				if (NumActiveControls == 0)
				{
					// start going to inactive
					State = State_CountingDownToInactive;
					Countdown = TimeUntilDeactive;
				}
				break;

			case State_CountingDownToInactive:
				// update the countdown
				Countdown -= InDeltaTime;
				if (Countdown <= 0.0f)
				{
					// should we start counting down to a control reset?
					if (TimeUntilReset > 0.0f)
					{
						State = State_CountingDownToReset;
						Countdown = TimeUntilReset;
					}
					else
					{
						// if not, then just go inactive
						State = State_Inactive;
					}
				}
				break;

			case State_CountingDownToReset:
				Countdown -= InDeltaTime;
				if (Countdown <= 0.0f)
				{
					// reset all the controls
					for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
					{
						//Controls[ControlIndex].Reset();
						//VisualCenter = CorrectedCenter;
						Controls[ControlIndex].VisualCenter = Controls[ControlIndex].CorrectedCenter;
					}

					// finally, go inactive
					State = State_Inactive;
				}
				break;
		}
	}
}

FReply SSmallEyeJoyStrick::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	if(IsValid(m_pJoyStrick))
	{
		USmallEyeJoyStrick* pSmalleyejoystric = Cast<USmallEyeJoyStrick>(m_pJoyStrick);
		if(pSmalleyejoystric)
			pSmalleyejoystric->NativeJoystickStart(true);
	}	
	_bTouching = true;
	FVector2D LocalCoord = MyGeometry.AbsoluteToLocal(Event.GetScreenSpacePosition());
	_lastMovedPos = LocalCoord;
	return  SVirtualJoystick::OnTouchStarted(MyGeometry, Event);
}

FReply SSmallEyeJoyStrick::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	FVector2D LocalCoord = MyGeometry.AbsoluteToLocal(Event.GetScreenSpacePosition());
	
	for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
	{
		FControlData& Control = Controls[ControlIndex];

		// is this control the one captured to this pointer?
		if (Control.CapturedPointerIndex == Event.GetPointerIndex())
		{			
			if (Control.bNeedUpdatedCenter)
			{
				return FReply::Unhandled();
			}
			else if (HandleTouch(ControlIndex, LocalCoord, MyGeometry.GetLocalSize()))
			{
								
				//if(_deadZone > 0)
				//{					
				//	FVector2D MoveDistance = LocalCoord - _lastMovedPos;
				//	if(FMath::Abs(MoveDistance.X) >= _deadZone || FMath::Abs(MoveDistance.Y) >= _deadZone)
				//	{							
				//		SetInputAxis(Control, ControlIndex, LocalCoord);
				//		_lastMovedPos = LocalCoord;
				//	}
				//		
				//}
				//else
				//{
					SetInputAxis(Control, ControlIndex, LocalCoord);
					_lastMovedPos = LocalCoord;
				//}
				
				
				return FReply::Handled();
			}
		}
	}

	return FReply::Unhandled();
}


FReply SSmallEyeJoyStrick::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	_bTouching =false;
	if(IsValid(m_pJoyStrick))
	{
		USmallEyeJoyStrick* pSmalleyejoystric = Cast<USmallEyeJoyStrick>(m_pJoyStrick);
		if(pSmalleyejoystric)
			pSmalleyejoystric->NativeJoystickEnd(true);
	}
	for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
	{
		FControlData& Control = Controls[ControlIndex];

		// is this control the one captured to this pointer?
		if (Control.CapturedPointerIndex == Event.GetPointerIndex())
		{
			Control.CapturedPointerIndex = -1;

			// release and center the joystick
			Control.ThumbPosition = FVector2D(0, 0);

			// send one more joystick update for the centering
			Control.bSendOneMoreEvent = true;

			// Pass event as unhandled if time is too short
			if (Control.bNeedUpdatedCenter)
			{
				Control.bNeedUpdatedCenter = false;
				return FReply::Unhandled();
			}			

			return FReply::Handled().ReleaseMouseCapture();
		}
	}

	return FReply::Unhandled();
}

int32 SSmallEyeJoyStrick::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 RetLayerId = LayerId;

	if (bVisible)
	{
		FLinearColor ColorAndOpacitySRGB = InWidgetStyle.GetColorAndOpacityTint();
		ColorAndOpacitySRGB.A = CurrentOpacity;

		for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
		{
			const FControlData& Control = Controls[ControlIndex];

			if (Control.Info.Image2.IsValid() &&  _bTouching)
			{
				FSlateDrawElement::MakeBox(
                    OutDrawElements,
                    RetLayerId++,
                    AllottedGeometry.ToPaintGeometry(
                    Control.VisualCenter - FVector2D(Control.CorrectedVisualSize.X * 0.5f, Control.CorrectedVisualSize.Y * 0.5f),
                    Control.CorrectedVisualSize),
                    Control.Info.Image2->GetSlateBrush(),
                    ESlateDrawEffect::None,
                    ColorAndOpacitySRGB
                    );
			}

			if (Control.Info.Image1.IsValid())
			{
				FSlateDrawElement::MakeBox(
                    OutDrawElements,
                    RetLayerId++,
                    AllottedGeometry.ToPaintGeometry(
                    Control.VisualCenter + Control.ThumbPosition - FVector2D(Control.CorrectedThumbSize.X * 0.5f, Control.CorrectedThumbSize.Y * 0.5f),
                    Control.CorrectedThumbSize),
                    Control.Info.Image1->GetSlateBrush(),
                    ESlateDrawEffect::None,
                    ColorAndOpacitySRGB
                    );
			}
		}
	}
	
	return RetLayerId;
}

void SSmallEyeJoyStrick::SetInputAxis(FControlData& Control, int32 ControlIndex, FVector2D& lastPos)
{
	
	FVector2D NormalizedOffset = lastPos - _lastMovedPos;
	NormalizedOffset *= 0.01f;

	float slopValue = _inputScale;
	if (FMath::Abs(NormalizedOffset.X) <= _deadZone/100 && FMath::Abs(NormalizedOffset.Y) <= _deadZone/100)
	{
		slopValue = 0;
	}
	
	NormalizedOffset.X = NormalizedOffset.X * fInitValue * (1+CalcChangeValue(NormalizedOffset.X,0.6f,slopValue));
	NormalizedOffset.Y = NormalizedOffset.Y * fPitchInputScale * fInitValue * (1+CalcChangeValue(NormalizedOffset.Y,0.6f,slopValue));
	const FGamepadKeyNames::Type XAxis = (Control.Info.MainInputKey.IsValid()
                                              ? Control.Info.MainInputKey.GetFName()
                                              : (ControlIndex == 0
                                                     ? FGamepadKeyNames::LeftAnalogX
                                                     : FGamepadKeyNames::RightAnalogX));
	const FGamepadKeyNames::Type YAxis = (Control.Info.AltInputKey.IsValid()
                                              ? Control.Info.AltInputKey.GetFName()
                                              : (ControlIndex == 0
                                                     ? FGamepadKeyNames::LeftAnalogY
                                                     : FGamepadKeyNames::RightAnalogY));

	if(IsValid(m_pJoyStrick)  && m_pJoyStrick->GetWorld())
	{
		APlayerController * pController = m_pJoyStrick->GetWorld()->GetGameInstance()->GetFirstLocalPlayerController();;
		if(pController)
		{
			static  float fAutoRunDeltaTime = 1 / 60.f;
			FKey keyx(XAxis);
			pController->InputAxis(keyx, NormalizedOffset.X, fAutoRunDeltaTime, 1, keyx.IsGamepadKey());
			FKey keyy(YAxis);
			pController->InputAxis(keyy, -NormalizedOffset.Y, fAutoRunDeltaTime, 1, keyy.IsGamepadKey());
			return;
		}
	}
	
	NormalizedOffset.Y = 0 - NormalizedOffset.Y;
	FSlateApplication::Get().OnControllerAnalog(XAxis, ControlIndex, NormalizedOffset.X);
	FSlateApplication::Get().OnControllerAnalog(YAxis, ControlIndex, -NormalizedOffset.Y);
}

void SSmallEyeJoyStrick::ReleaseJoystick()
{
	_bTouching =false;
	for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
	{
		FControlData& Control = Controls[ControlIndex];
		// release all control
		{
			Control.CapturedPointerIndex = -1;

			// release and center the joystick
			Control.ThumbPosition = FVector2D(0, 0);

			// send one more joystick update for the centering
			Control.bSendOneMoreEvent = true;

			// Pass event as unhandled if time is too short
			if (Control.bNeedUpdatedCenter)
			{
				Control.bNeedUpdatedCenter = false;
			}
		}
	}
}

void SSmallEyeJoyStrick::SetTurnValue(float fInDeadZone, float fInInitValue, float fInPitchInputScale)
{
	_deadZone = fInDeadZone;
	fInitValue = fInInitValue;
	fPitchInputScale = fInPitchInputScale;
}

float SSmallEyeJoyStrick::CalcChangeValue(float fInCurrent, float fInDuration, float fInChangeValue)
{
	fInCurrent = FMath::Abs(fInCurrent);
	fInCurrent = fInCurrent / fInDuration;
	if (fInCurrent > 1)
	{
		fInCurrent = 1;
	}
	return fInChangeValue * fInCurrent * fInCurrent;
}