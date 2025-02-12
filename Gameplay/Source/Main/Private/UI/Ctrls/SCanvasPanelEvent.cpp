#include "SCanvasPanelEvent.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Slate/UMGDragDropOp.h"
#include "CanvasPanelEvent.h"

SCanvasPanelEvent::SCanvasPanelEvent()
{
	m_pWidget = nullptr;
	_bOnFocus = false;
}

FReply SCanvasPanelEvent::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FUMGDragDropOp> NativeOp = DragDropEvent.GetOperationAs<FUMGDragDropOp>();
	if ( NativeOp.IsValid() )
	{
		
		{
			if ( m_pWidget->NativeOnDrop( MyGeometry, DragDropEvent, NativeOp->GetOperation() ) )
			{
				return FReply::Handled();
			}
		}
	}

	return FReply::Unhandled();
}

void SCanvasPanelEvent::OnFocusLost(const FFocusEvent& InFocusEvent)
{
}

void SCanvasPanelEvent::OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{	
	bool bPreOnfocus = _bOnFocus;
	bool bNewFocus = false;
	for(int32 WidgetIndex = 0; WidgetIndex < NewWidgetPath.Widgets.Num(); ++WidgetIndex)
	{
		if ( &NewWidgetPath.Widgets[WidgetIndex].Widget.Get() == this )
		{
			bNewFocus = true;
			break;
		}
	}
	_bOnFocus = bNewFocus ;//NewWidgetPath.ContainsWidget(TSharedRef<SWidget>(this));
	if(bPreOnfocus && !_bOnFocus)
	{
		if(IsValid(m_pWidget))
		{
			m_pWidget->NativeOnFocusLost();		
		}
	}
	
}


bool SCanvasPanelEvent::SupportsKeyboardFocus() const
{
	return  true;
}