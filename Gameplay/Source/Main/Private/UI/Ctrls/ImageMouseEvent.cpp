#include "ImageMouseEvent.h"


#include "SImageMouseEvent.h"
#include "Components/CanvasPanelSlot.h"
#include "Widgets/Images/SImage.h"

UImageMouseEvent::UImageMouseEvent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	_nEffectButtonIndex = -1;
}

void UImageMouseEvent::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (MyImage.IsValid())
	{
		if(_handleMouseButtonUp || _handleMouseButtonUpNotNeedDown)
		{
			MyImage->SetOnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonUp));
			MyImage->SetOnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonDownEx));
		}
		if(_handleMouseMove)
		{
			MyImage->SetOnMouseMove(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseMove));
			MyImage->SetOnMouseEnter(BIND_UOBJECT_DELEGATE(FNoReplyPointerEventHandler, HandleMouseEnter));
			MyImage->SetOnMouseLeave(BIND_UOBJECT_DELEGATE(FSimpleNoReplyPointerEventHandler, HandleMouseLeave));
		}
		
		SImageMouseEvent* pImage = (SImageMouseEvent*)(MyImage.Get());
		if(pImage)
		{
			pImage->SetOnMouseWheel(BIND_UOBJECT_DELEGATE(FOnDragDetected, HandleMouseWheel));
			pImage->_SyncInputTouch = _SyncInputTouch;
			pImage->m_pUWidget = this;
		}
	}
}

FReply UImageMouseEvent::HandleMouseMove(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if(_bMouseMoveShuoldDown)
	{
		if(_nEffectButtonIndex == -1)
			return  FReply::Unhandled();
	}
	if(OnMouseMoveEvent.IsBound())
	{
		FVector2D inter = MouseEvent.GetScreenSpacePosition() - _lastMovePos;
		_lastMovePos = MouseEvent.GetScreenSpacePosition();
		if(Geometry.Scale != 0)
			inter /= Geometry.Scale;
		return  OnMouseMoveEvent.Execute(MouseEvent.GetScreenSpacePosition(),  inter, _nEffectButtonIndex).NativeReply;
	}
	return  FReply::Unhandled();
}

void UImageMouseEvent::HandleMouseEnter(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	
	if ( OnMouseEnterEvent.IsBound() )
	{
		_lastMovePos = MouseEvent.GetScreenSpacePosition();
		FVector2D localPos = GetLocationPos(Geometry, _lastMovePos);
		OnMouseEnterEvent.Execute(localPos, MouseEvent.GetCursorDelta(),0);
	}
}

void UImageMouseEvent::HandleMouseLeave(const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();
	if ( Reply.GetMouseCaptor().IsValid() == false && HasMouseCapture() )
	{
		return;
	}
	_nEffectButtonIndex = -1;
	if(OnMouseLeaveEvent.IsBound())
	{
		OnMouseLeaveEvent.Execute(MouseEvent.GetScreenSpacePosition(), MouseEvent.GetCursorDelta(),0);
	}
}

FReply UImageMouseEvent::HandleMouseWheel(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();
	if(OnMouseWheelEvent.IsBound())
	{
		OnMouseWheelEvent.Execute(MouseEvent.GetScreenSpacePosition(), MouseEvent.GetGestureDelta(),0);
		return  FReply::Handled();
	}
	return  Reply;
}


TSharedRef<SWidget> UImageMouseEvent::RebuildWidget()
{
	MyImage = SNew(SImageMouseEvent)
            .FlipForRightToLeftFlowDirection(bFlipForRightToLeftFlowDirection);

	return MyImage.ToSharedRef();
}

FReply UImageMouseEvent::HandleMouseButtonDownEx(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	_lastMovePos = MouseEvent.GetScreenSpacePosition();
	FReply Reply = FReply::Unhandled();
	if (_nEffectButtonIndex == -1)
	{
		_nEffectButtonIndex = GetEffectButtonIndex(MouseEvent);
		if(OnMouseButtonDownEventEx.IsBound())
		{
			FVector2D localPos = GetLocationPos(Geometry, _lastMovePos);
			Reply = OnMouseButtonDownEventEx.Execute(localPos, MouseEvent.GetCursorDelta(), _nEffectButtonIndex).NativeReply;
			Reply.CaptureMouse(GetCachedWidget().ToSharedRef());
		}
	}
	return  Reply;
}


FReply UImageMouseEvent::HandleMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();
	int nButtonIndex = GetEffectButtonIndex(MouseEvent);	
	if(nButtonIndex == _nEffectButtonIndex || _handleMouseButtonUpNotNeedDown)
	{	
		if (OnMouseButtonUpEvent.IsBound())
		{
			FVector2D localPos = GetLocationPos(Geometry, MouseEvent.GetScreenSpacePosition());
			Reply = OnMouseButtonUpEvent.Execute(localPos, MouseEvent.GetCursorDelta(),nButtonIndex).
			                             NativeReply;
		}		
	}
	if (nButtonIndex == _nEffectButtonIndex)
	{
		_nEffectButtonIndex = -1;
		if (Reply.GetMouseCaptor().IsValid() == false && HasMouseCapture())
		{
			Reply.ReleaseMouseCapture();
		}
	}
	
	return  Reply;
}

FVector2D UImageMouseEvent::GetLocationPos(const FGeometry& Geometry, FVector2D screenPos)
{
	FVector2D localPos = Geometry.AbsoluteToLocal(screenPos);
	UCanvasPanelSlot* pCanvasSlot = Cast<UCanvasPanelSlot>(Slot);
	if(pCanvasSlot)
	{
		FVector2D aligent = pCanvasSlot->GetAlignment();
		localPos -= Geometry.Size * aligent;
	}
	return localPos;
}

void UImageMouseEvent::DrawLine(bool bDraw, FVector2D pos1, FVector2D pos2, float fInter, float nPerDis, float Thickness, FLinearColor color)
{
	if (MyImage.IsValid())
	{
		SImageMouseEvent* pImage = (SImageMouseEvent*)(MyImage.Get());
		if(pImage)
		{
			pImage->SetDrawLine(bDraw, pos1, pos2, fInter, nPerDis, Thickness, color);
		}
	}
}

void UImageMouseEvent::ReleasePress()
{
	_nEffectButtonIndex = -1;
}

int UImageMouseEvent::GetEffectButtonIndex(const FPointerEvent& MouseEvent)
{
	if(MouseEvent.IsTouchEvent())
		return 0;
	FKey effectBtn = MouseEvent.GetEffectingButton();
	if (effectBtn == EKeys::LeftMouseButton)
		return  0;
	if(effectBtn == EKeys::MiddleMouseButton)
		return 1;
	return 2;
}
