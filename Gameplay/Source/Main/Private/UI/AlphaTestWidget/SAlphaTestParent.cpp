

#include "SAlphaTestParent.h"
#include "SAlphaTestButton.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

SAlphaTestParent::SAlphaTestParent()
	: HittestGrid(MakeUnique<FAlphaTestHittestGrid>())
{
	
}

bool SAlphaTestParent::SupportsKeyboardFocus() const
{
	return true;
}

FReply SAlphaTestParent::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) 
{
	bMovedWhenPress = 0;
	int32 TouchIndex = MouseEvent.IsTouchEvent() ? MouseEvent.GetTouchpadIndex() : 0;
	if (HittestGrid.IsValid())
	{
		TArray<TSharedPtr<SWidget>> Path = HittestGrid->GetAppropriateWidget(MyGeometry,MouseEvent);
		FReply Result = FReply::Unhandled();

		for(int i = 0; i < Path.Num(); i++)
		{
			TSharedPtr<SWidget> CurWidget = Path[i];
			if (CurWidget.Get() == this)
			{
				break;
			}

			Result = CurWidget->OnMouseButtonDown(CurWidget->GetCachedGeometry(),MouseEvent);
			if (Result.IsEventHandled())
			{
				CurFocusWidget.Emplace(TouchIndex,Path[i]);
				SBorder::OnMouseButtonDown(MyGeometry, MouseEvent);
				return FReply::Handled().CaptureMouse( AsShared() );
			}
		}
	}

	CurFocusWidget.Remove(TouchIndex);
	return SBorder::OnMouseButtonDown(MyGeometry, MouseEvent);
}
 
FReply SAlphaTestParent::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) 
{
	TSharedPtr<SWidget> CurWidget;
	CurFocusWidget.RemoveAndCopyValue(MouseEvent.GetPointerIndex(),CurWidget);
	if (CurWidget.IsValid())
	{
		FReply Result = CurWidget->OnMouseButtonDoubleClick(CurWidget->GetCachedGeometry(),MouseEvent);
		if (Result.IsEventHandled())
		{
			SBorder::OnMouseButtonDoubleClick(MyGeometry, MouseEvent);
			return FReply::Handled().CaptureMouse( AsShared() );
		}
	}
	return SBorder::OnMouseButtonDoubleClick(MyGeometry, MouseEvent);
}
 
FReply SAlphaTestParent::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) 
{
	TSharedPtr<SWidget> CurWidget;
	int32 TouchIndex = MouseEvent.IsTouchEvent() ? MouseEvent.GetTouchpadIndex() : 0;
	CurFocusWidget.RemoveAndCopyValue(TouchIndex,CurWidget);
	if (CurWidget.IsValid())
	{
		if (!bMovedWhenPress)
		{
			FReply Result = CurWidget->OnMouseButtonUp(CurWidget->GetCachedGeometry(),MouseEvent);
			if (Result.IsEventHandled())
			{
				SBorder::OnMouseButtonUp(MyGeometry, MouseEvent);
				return FReply::Handled().ReleaseMouseCapture();
			}
			else
			{
				SBorder::OnMouseButtonUp(MyGeometry, MouseEvent);
				return FReply::Unhandled().ReleaseMouseCapture();
			}
		}
		else
		{
			SAlphaTestButton* CurIsButton = reinterpret_cast<SAlphaTestButton*>(CurWidget.Get());
			if (CurIsButton)
			{
				CurIsButton->CallButtonRelease();
			}
		}
	}
	return SBorder::OnMouseButtonUp(MyGeometry, MouseEvent).ReleaseMouseCapture();
}

FReply SAlphaTestParent::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) 
{
	int32 UserIndex = MouseEvent.IsTouchEvent() ? MouseEvent.GetTouchpadIndex() : 0;
	/*Mouse enter or leave.-Start-*/
	if (!MouseEvent.GetCursorDelta().IsNearlyZero())
	{
		bMovedWhenPress = 1;
		TArray<TSharedPtr<SWidget>> Path = HittestGrid->GetAppropriateWidget(MyGeometry,MouseEvent);
		TSet<int32> LeavedWidget;

		TArray<TSharedPtr<SWidget>>* LastEnterWidgets_Array = nullptr;
		if (CurMouseEnterWidgets.Contains(UserIndex))
		{
			LastEnterWidgets_Array = CurMouseEnterWidgets.Find(UserIndex);

			for (int i = 0; i < LastEnterWidgets_Array->Num(); i++)
			{
				TSharedPtr<SWidget> LastEnterWidget = (*LastEnterWidgets_Array)[i];
				if (!Path.Contains(LastEnterWidget))
				{
					LastEnterWidget->OnMouseLeave(MouseEvent);
					LastEnterWidgets_Array->RemoveAt(i);
					i--;
				}
			}
		}
		else
		{
			 LastEnterWidgets_Array = &CurMouseEnterWidgets.Emplace(UserIndex,TArray<TSharedPtr<SWidget>>());
		}
		
		for(int i = 0; i < Path.Num(); i++)
		{
			TSharedPtr<SWidget> CurWidget = Path[i];
			if (CurWidget.Get() == this)
			{
				break;
			}
			
			if (!LastEnterWidgets_Array->Contains(CurWidget))
			{
				CurWidget->OnMouseEnter(MyGeometry, MouseEvent);
				LastEnterWidgets_Array->Add(CurWidget);
			}
		}
		
	}
	/*Mouse enter or leave.-End-*/
	
	if (TSharedPtr<SWidget>* CurWidget = CurFocusWidget.Find(UserIndex))
	{
		if (CurWidget->IsValid())
		{
			FReply Result =  CurWidget->Get()->OnMouseMove(CurWidget->Get()->GetCachedGeometry(),MouseEvent);
			if (Result.IsEventHandled())
			{
				return Result;
			}
		}
	}
	
	return SBorder::OnMouseMove(MyGeometry, MouseEvent);
}

int32 SAlphaTestParent::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	//Call SCompoundWidget::OnPaint not SBorder::OnPaint, don't draw image.
	int32 FinalLayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	HittestGrid->Clear();
	const bool HittestCleared = HittestGrid->SetHittestArea( GetDesiredSize());
	SAlphaTestParent* cc = const_cast<SAlphaTestParent*>(this);

	int32 SimulateLayerId = 0;
	AddWidgetToGrid(cc->GetChildren()->GetChildAt(0),SimulateLayerId);
	
	return FinalLayerId;
}

void SAlphaTestParent::AddWidgetToGrid(TSharedRef<SWidget> InAddWidget, int32 SimulateLayerId) const
{
	if (InAddWidget->GetVisibility() == EVisibility::Visible)
	{
		SWidget* MutableThis = const_cast<SWidget*>(&InAddWidget.Get());
		
		HittestGrid->AddWidget(MutableThis, 0, SimulateLayerId/*, FastPathProxyHandle.GetWidgetSortOrder()*/); 
		SimulateLayerId++;
	}

	FChildren* WidgetChildren = InAddWidget->GetChildren();
	if (WidgetChildren)
	{
		for (int i = 0; i < WidgetChildren->Num(); i++)
		{
			TSharedRef<SWidget> CurWidget = WidgetChildren->GetChildAt(i);
			AddWidgetToGrid(CurWidget,SimulateLayerId);
		}
	}
}

FReply SAlphaTestParent::OnMouseWheel( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	if(OnMouseWheelEvent.IsBound())
		return OnMouseWheelEvent.Execute(MyGeometry, MouseEvent);
	
	return FReply::Unhandled();
}

void SAlphaTestParent::SetOnMouseWheel(FOnDragDetected EventHandler)
{
	OnMouseWheelEvent = EventHandler;
}

//FReply SAlphaTestParent::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
//{
//	return OnMouseButtonDown(MyGeometry,InTouchEvent);
//}
//
//FReply SAlphaTestParent::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
//{
//	return OnMouseButtonUp(MyGeometry,InTouchEvent);
//}
//
//FReply SAlphaTestParent::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& InTouchEvent)
//{
//	return OnMouseMove(MyGeometry,InTouchEvent);
//}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
