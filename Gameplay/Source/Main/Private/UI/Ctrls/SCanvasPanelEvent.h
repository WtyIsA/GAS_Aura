#pragma once
#include "Widgets/Layout/SConstraintCanvas.h"

class UCanvasPanelEvent;

class SCanvasPanelEvent : public  SConstraintCanvas
{
public:
	SCanvasPanelEvent();
	UCanvasPanelEvent*	m_pWidget;
	virtual bool SupportsKeyboardFocus() const;
private:
	//virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent);
	virtual void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent);

	bool _bOnFocus;
};
