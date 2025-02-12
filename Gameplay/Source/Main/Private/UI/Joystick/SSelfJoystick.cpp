#include "SSelfJoystick.h"



#include "SelfJoystick.h"
#include "UtilsProject.h"
#include "Engine/LocalPlayer.h"
#include "Framework/Application/SlateApplication.h"

SSelfJoystick::SSelfJoystick():SSelfBaseJoystick()
{
	_rushDisASqr = 0.0f;
	_bShowRun = false;
	_inputScale = 1.0f;
	_bRushing = false;
	_bCanAutoRush = true;
	_bThumbBarFix = false;
}

void SSelfJoystick::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
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

		if (Control.CapturedPointerIndex >= 0 || Control.bSendOneMoreEvent || _bThumbBarFix)
		{
			Control.bSendOneMoreEvent = false;			
			SetInputAxis(Control, ControlIndex);			
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

FReply SSelfJoystick::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	if(_bThumbBarFix)
	{
		_bThumbBarFix = false;
		if(IsValid(m_pJoyStrick))
		{
			USelfJoyStrick* pSelfJoyStrick = Cast<USelfJoyStrick>(m_pJoyStrick);
			if(pSelfJoyStrick)
				pSelfJoyStrick->NativeRunMouseUp(false);
		}
	}
	if(IsValid(m_pJoyStrick))
	{
		USelfJoyStrick* pSelfJoyStrick = Cast<USelfJoyStrick>(m_pJoyStrick);
		if(pSelfJoyStrick)
			pSelfJoyStrick->NativeJoystickStart(true);
	}
	return  SVirtualJoystick::OnTouchStarted(MyGeometry, Event);
}

FReply SSelfJoystick::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	FVector2D LocalCoord = MyGeometry.AbsoluteToLocal(Event.GetScreenSpacePosition());
	
	for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
	{
		FControlData& Control = Controls[ControlIndex];

		// is this control the one captured to this pointer?
		if (Control.CapturedPointerIndex == Event.GetPointerIndex())
		{
			if(_bCanAutoRush)
			{
				bool bInRun = FUtilsProject::PointInRect(ShowRunMargin, LocalCoord);
				if(bInRun != _bShowRun)
				{
					_bShowRun = bInRun;
					USelfJoyStrick* pSelfJoyStrick = Cast<USelfJoyStrick>(m_pJoyStrick);
					if(pSelfJoyStrick)
						pSelfJoyStrick->NativeShowRun(_bShowRun);
				}
			}
			if (Control.bNeedUpdatedCenter)
			{
				return FReply::Unhandled();
			}
			else if (HandleTouch(ControlIndex, LocalCoord, MyGeometry.GetLocalSize()))
			{				
				if (/*_bCanAutoRush &&*/ _rushDisASqr > 0)
				{
					if( _fThumbDisSqr >= _rushDisASqr)
					{
						if(!_bRushing)
						{
							_bRushing = true;
							USelfJoyStrick* pSelfJoyStrick = Cast<USelfJoyStrick>(m_pJoyStrick);
							if(pSelfJoyStrick)
								pSelfJoyStrick->NativeRush(_bRushing);
						}
					}
					else
					{
						if(_bRushing)
						{
							_bRushing = false;
							USelfJoyStrick* pSelfJoyStrick = Cast<USelfJoyStrick>(m_pJoyStrick);
							if(pSelfJoyStrick)
								pSelfJoyStrick->NativeRush(_bRushing);
						}
					}
				}
				return FReply::Handled();
			}
		}
	}

	return FReply::Unhandled();
}


FReply SSelfJoystick::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	FVector2D LocalCoord = MyGeometry.AbsoluteToLocal(Event.GetScreenSpacePosition());
	if(_bCanAutoRush)
	{
		_bThumbBarFix = FUtilsProject::PointInRect(RunCtrlRect, LocalCoord);
		if(_bThumbBarFix)
		{
			if(IsValid(m_pJoyStrick))
			{
				USelfJoyStrick* pSelfJoyStrick = Cast<USelfJoyStrick>(m_pJoyStrick);
				if(pSelfJoyStrick)
					pSelfJoyStrick->NativeRunMouseUp(true);
			}
		}
		else if(_bShowRun)
		{
			_bShowRun = false;
			USelfJoyStrick* pSelfJoyStrick = Cast<USelfJoyStrick>(m_pJoyStrick);
			if(pSelfJoyStrick)
				pSelfJoyStrick->NativeShowRun(_bShowRun);
		}
	}
	if(IsValid(m_pJoyStrick))
	{
		USelfJoyStrick* pSelfJoyStrick = Cast<USelfJoyStrick>(m_pJoyStrick);
		if(pSelfJoyStrick)
			pSelfJoyStrick->NativeJoystickEnd(true);
	}
	for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
	{
		FControlData& Control = Controls[ControlIndex];

		// is this control the one captured to this pointer?
		if (Control.CapturedPointerIndex == Event.GetPointerIndex())
		{
			Control.CapturedPointerIndex = -1;
			if(!_bThumbBarFix)
			{
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
			}else
			{
				LocalCoord.X = Control.VisualCenter.X;
				HandleTouch(ControlIndex, LocalCoord, MyGeometry.GetLocalSize());
			}

			return FReply::Handled().ReleaseMouseCapture();
		}
	}

	return FReply::Unhandled();
}

void SSelfJoystick::SetThumbBarFix(bool bFix)
{
	_bThumbBarFix = bFix;
	// if not fixed,reset all controls's ThumbPosition.
	if (!bFix)
	{
		for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
		{
			FControlData& Control = Controls[ControlIndex];

			{
				// release and center the joystick
				Control.ThumbPosition = FVector2D(0, 0);
				Control.CapturedPointerIndex = -1;

				// send one more joystick update for the centering
				Control.bSendOneMoreEvent = true;

				if (Control.bNeedUpdatedCenter)
				{
					Control.bNeedUpdatedCenter = false;
				}

				// Set input immediately.The widget may hide.
				SetInputAxis(Control, ControlIndex);
			}
		}
	}
}

int32 SSelfJoystick::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 RetLayerId = LayerId;

	if (bVisible)
	{
		FLinearColor ColorAndOpacitySRGB = InWidgetStyle.GetColorAndOpacityTint();
		ColorAndOpacitySRGB.A = CurrentOpacity;

		for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
		{
			const FControlData& Control = Controls[ControlIndex];

			if (Control.Info.Image2.IsValid() )
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

void SSelfJoystick::SetInputAxis(FControlData& Control, int32 ControlIndex)
{
	FVector2D ThumbOffset = Control.ThumbPosition;

	if(FMath::Abs(ThumbOffset.X) > FMath::Abs(ThumbOffset.Y) )
	{
		ThumbOffset.Y = ThumbOffset.Y / ThumbOffset.X;
		if(ThumbOffset.X > 0)
		{	
			ThumbOffset.X = 1;
		}
		else
		{
			ThumbOffset.X = -1;
			ThumbOffset.Y = 0 - ThumbOffset.Y;
		}
	}
	else
	{
		if(ThumbOffset.Y != 0)
		{
			ThumbOffset.X = ThumbOffset.X / ThumbOffset.Y;
			if(ThumbOffset.Y > 0)
				ThumbOffset.Y = 1;
			else
			{
				ThumbOffset.Y = -1;
				ThumbOffset.X = 0 - ThumbOffset.X;
			}
		}
	}
	
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
			pController->InputAxis(keyx, ThumbOffset.X, fAutoRunDeltaTime, 1, keyx.IsGamepadKey());
			FKey keyy(YAxis);
			pController->InputAxis(keyy, -ThumbOffset.Y, fAutoRunDeltaTime, 1, keyy.IsGamepadKey());
			return;
		}
	}
	FSlateApplication::Get().OnControllerAnalog(XAxis, ControlIndex, ThumbOffset.X);
	FSlateApplication::Get().OnControllerAnalog(YAxis, ControlIndex, -ThumbOffset.Y);
}

FVector2D SSelfJoystick::GetThumbPos(int32 ControlIndex)
{
	if (Controls.IsValidIndex(ControlIndex))
	{
		return Controls[ControlIndex].ThumbPosition;
	}
	return FVector2D(0,0);
}

void SSelfJoystick::ReleaseJoystick()
{
	for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
	{
		FControlData& Control = Controls[ControlIndex];
		{
			// release and center the joystick
			Control.ThumbPosition = FVector2D(0, 0);
			Control.CapturedPointerIndex = -1;

			// send one more joystick update for the centering
			Control.bSendOneMoreEvent = true;

			if (Control.bNeedUpdatedCenter)
			{
				Control.bNeedUpdatedCenter = false;
			}

			// Set input immediately.The widget may hide.
			SetInputAxis(Control, ControlIndex);
		}
	}
}