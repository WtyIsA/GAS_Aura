#pragma once
#include "Widgets/Input/SVirtualJoystick.h"


class UWidget;

class  USelfBaseJoyStrick;

class  SSelfBaseJoystick : public SVirtualJoystick
{
public:
	SSelfBaseJoystick();
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event) override;

	USelfBaseJoyStrick*   m_pJoyStrick;
	void  SetInputScale(float inputScale){_inputScale = inputScale;}

	void SetActiveOpacity(float fActiveOpacity);

protected:
	FVector2D ComputeThumbPosition(int32 ControlIndex, const FVector2D& LocalCoord, float* OutDistanceToTouchSqr = nullptr, float* OutDistanceToEdgeSqr = nullptr);
    virtual bool HandleTouch(int32 ControlIndex, const FVector2D& LocalCoord, const FVector2D& ScreenSize);

	int32 ResolveRelativePosition(float Position, float RelativeTo, float ScaleFactor);
	float GetBaseOpacity();
	float _fThumbDisSqr;
	float _inputScale;
};
