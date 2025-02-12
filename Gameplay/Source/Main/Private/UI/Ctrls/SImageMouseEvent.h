#pragma once
#include <Widgets/Images/SImage.h>
#include "SlateWrapperTypesEx.h"

class UWidget;

class MAIN_API SImageMouseEvent : public SImage
{
public:
    SImageMouseEvent();

    /** See OnMouseButtonDown event */
    void SetOnMouseWheel(FOnDragDetected EventHandler);
    virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
    virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
    virtual FReply OnTouchStarted( const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent ) override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	bool _SyncInputTouch;
	UWidget* m_pUWidget;
	void SetDrawLine(bool bDraw, FVector2D pos1 = FVector2D::ZeroVector,FVector2D pos2 = FVector2D::ZeroVector,float fInter=0,float nPerDis = 1, float Thickness =1, FLinearColor  color = FLinearColor::White);
private:
    FOnDragDetected  _OnMouseWheel;

	bool _bDrawLine;
	float _fThickness;
	TArray<TArray<FVector2D>> _Points;
	FVector2D	_linePos1;
	FVector2D	_linePos2;
	FLinearColor  _lineColor;
};
