//gjy
#pragma once

#include "CoreMinimal.h"
#include "AlphaTestHittestGrid.h"
#include "Widgets/Layout/SConstraintCanvas.h"

/*
 * Call SCompoundWidget::OnPaint not SBorder::OnPaint, don't draw image
 * Must with SAlphaTestButton
 */
class SAlphaTestParent : public SBorder
{
public:
	SAlphaTestParent();
	
	virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseWheel( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;

	void SetOnMouseWheel(FOnDragDetected EventHandler);

	// virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	// virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	// virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent) override;
	
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	/**
	 * Checks to see if this widget supports keyboard focus.  Override this in derived classes.
	 *
	 * @return  True if this widget can take keyboard focus
	 */
	virtual bool SupportsKeyboardFocus() const override;
private:
	
	FOnDragDetected OnMouseWheelEvent;
	/** Each AlphaTestParent has its own hittest grid for accelerated widget picking. */
	TUniquePtr<FAlphaTestHittestGrid> HittestGrid;
	TMap<int32,TSharedPtr<SWidget>> CurFocusWidget;
	TMap<int32,TArray<TSharedPtr<SWidget>>> CurMouseEnterWidgets;
	uint8 bMovedWhenPress:1;

	void AddWidgetToGrid(TSharedRef<SWidget> InAddWidget, int32 SimulateLayerId) const;
};
