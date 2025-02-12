#pragma once
#include "SSelfBaseJoystick.h"
#include "Widgets/Input/SVirtualJoystick.h"

class  SSmallEyeJoyStrick : public SSelfBaseJoystick
{
public:
	SSmallEyeJoyStrick();
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event) override;

	void SetDeadZone(float deadZone){_deadZone = deadZone;}
	void ReleaseJoystick();

	void SetTurnValue(float fInDeadZone, float fInInitValue, float fInPitchInputScale);
private:
	void  SetInputAxis(FControlData& Control, int32 ControlIndex, FVector2D& lastPos);
	float _deadZone;
	FVector2D _lastMovedPos;
	
	bool		_bTouching;
	
	float fInitValue = 1.0f;
	float fPitchInputScale = 1.0f;

	float CalcChangeValue(float fInCurrent, float fInDuration, float fInChangeValue);
};
