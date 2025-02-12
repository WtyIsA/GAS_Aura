#pragma once
#include "SSelfBaseJoystick.h"
#include "Widgets/Input/SVirtualJoystick.h"

class  SSelfJoystick : public SSelfBaseJoystick
{
public:
	SSelfJoystick();
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	void SetThumbBarFix(bool bFix);

	void SetShowRunMargin(FMargin& runMargin){ShowRunMargin = runMargin;}
	void SetRunCtrlRect(FMargin& runRect){ RunCtrlRect = runRect;}
	void SetAutoRush(bool bAutoRush){_bCanAutoRush = bAutoRush;}
	void SetRushDisAspr(float runDis){_rushDisASqr = runDis;}

	FVector2D GetThumbPos(int32 ControlIndex);

	void ReleaseJoystick();
	
private:
	void SetInputAxis(FControlData& Control, int32 ControlIndex);
	FMargin		ShowRunMargin;
	FMargin		RunCtrlRect;
	bool		_bCanAutoRush;
	float		_rushDisASqr;
	bool		_bThumbBarFix;
	bool		_bShowRun;
	bool		_bRushing;
};
