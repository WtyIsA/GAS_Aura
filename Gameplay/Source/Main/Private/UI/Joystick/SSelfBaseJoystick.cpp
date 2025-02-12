#include "SSelfBaseJoystick.h"

#include "Engine/LocalPlayer.h"

SSelfBaseJoystick::SSelfBaseJoystick()
{
	m_pJoyStrick = nullptr;
	_fThumbDisSqr = 0.0f;
	_inputScale = 1.0f;
}


int32 SSelfBaseJoystick::ResolveRelativePosition(float Position, float RelativeTo, float ScaleFactor)
{
	// absolute from edge
	if (Position < -1.0f)
	{
		return RelativeTo + Position * ScaleFactor;
	}
	// relative from edge
	else if (Position < 0.0f)
	{
		return RelativeTo + Position * RelativeTo;
	}
	// relative from 0
	else if (Position <= 1.0f)
	{
		return Position * RelativeTo;
	}
	// absolute from 0
	else
	{
		return Position * ScaleFactor;
	}

}


FVector2D SSelfBaseJoystick::ComputeThumbPosition(int32 ControlIndex, const FVector2D& LocalCoord, float* OutDistanceToTouchSqr, float* OutDistanceToEdgeSqr)
{
	float DistanceToTouchSqr = 0.0f;
	float DistanceToEdgeSqr = 0.0f;
	FVector2D Position;
	const FControlData& Control = Controls[ControlIndex];

	// figure out position around center
	FVector2D Offset = LocalCoord - Controls[ControlIndex].VisualCenter;
	// only do work if we aren't at the center
	if (Offset == FVector2D(0, 0))
	{
		Position = Offset;
	}
	else
	{
		// clamp to the ellipse of the stick (snaps to the visual size, so, the art should go all the way to the edge of the texture)
		DistanceToTouchSqr = Offset.SizeSquared();
		float Angle = FMath::Atan2(Offset.Y, Offset.X);

		// length along line to ellipse: L = 1.0 / sqrt(((sin(T)/Rx)^2 + (cos(T)/Ry)^2))
		float CosAngle = FMath::Cos(Angle);
		float SinAngle = FMath::Sin(Angle);
		float XTerm  =  (Control.CorrectedVisualSize.X * 0.5f);
		float YTerm  =  (Control.CorrectedVisualSize.Y * 0.5f);
		DistanceToEdgeSqr =  XTerm * XTerm;
		// only clamp 
		if (DistanceToTouchSqr > DistanceToEdgeSqr)
		{
			Position = FVector2D(XTerm * CosAngle, XTerm * SinAngle);
		}
		else
		{
			Position = Offset;
		}
	}

	if (OutDistanceToTouchSqr != nullptr)
	{
		*OutDistanceToTouchSqr = DistanceToTouchSqr;
	}

	if (OutDistanceToEdgeSqr != nullptr)
	{
		*OutDistanceToEdgeSqr = DistanceToEdgeSqr;
	}

	return Position;
}

bool SSelfBaseJoystick::HandleTouch(int32 ControlIndex, const FVector2D& LocalCoord, const FVector2D& ScreenSize)
{
	FControlData& Control = Controls[ControlIndex];
	Control.ThumbPosition = ComputeThumbPosition(ControlIndex, LocalCoord, &_fThumbDisSqr);

	FVector2D AbsoluteThumbPos = Control.ThumbPosition + Controls[ControlIndex].VisualCenter;
	AlignBoxIntoScreen(AbsoluteThumbPos, Control.CorrectedThumbSize, ScreenSize);
	Control.ThumbPosition = AbsoluteThumbPos - Controls[ControlIndex].VisualCenter;
	return true;
}

void SSelfBaseJoystick::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	
}

FReply SSelfBaseJoystick::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	return  SVirtualJoystick::OnTouchStarted(MyGeometry, Event);
}

FReply SSelfBaseJoystick::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event)
{	
	return FReply::Unhandled();
}


FReply SSelfBaseJoystick::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event)
{	
	return FReply::Unhandled();
}

int32 SSelfBaseJoystick::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	return  LayerId;
}

float SSelfBaseJoystick::GetBaseOpacity()
{
	return (State == State_Active || State == State_CountingDownToInactive) ? ActiveOpacity : InactiveOpacity;
}

void SSelfBaseJoystick::SetActiveOpacity(float fActiveOpacity)
{
	ActiveOpacity = fActiveOpacity;
}