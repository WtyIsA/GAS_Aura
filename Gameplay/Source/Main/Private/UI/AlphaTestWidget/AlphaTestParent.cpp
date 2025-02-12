// Fill out your copyright notice in the Description page of Project Settings.


#include "AlphaTestParent.h"

#include "Components/BorderSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

TSharedRef<SWidget> UAlphaTestParent::RebuildWidget()
{
	MyBorder = SNew(SAlphaTestParent)
		.FlipForRightToLeftFlowDirection(bFlipForRightToLeftFlowDirection);
	
	if ( GetChildrenCount() > 0 )
	{
		Cast<UBorderSlot>(GetContentSlot())->BuildSlot(MyBorder.ToSharedRef());
	GetContent()->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	
	return MyBorder.ToSharedRef();
}

void UAlphaTestParent::ReleasePress()
{
	_bMouseButtonDown = false;
}

FReply UAlphaTestParent::HandleMouseMove_AlphaTest(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if(_bMouseButtonDown && OnMouseMoveEventEx.IsBound())
	{
		FVector2D inter = MouseEvent.GetScreenSpacePosition() - _lastMovePos;
		_lastMovePos = MouseEvent.GetScreenSpacePosition();
		if(Geometry.Scale != 0)
			inter /= Geometry.Scale;
		return  OnMouseMoveEventEx.Execute(MouseEvent.GetScreenSpacePosition(),  inter).NativeReply;
	}
	return  Super::HandleMouseMove(Geometry,MouseEvent);
}

FReply UAlphaTestParent::HandleMouseButtonDown_AlphaTest(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	_bMouseButtonDown = true;
	_lastMovePos = MouseEvent.GetScreenSpacePosition();
	if (OnMouseButtonDownEventEx.IsBound())
	{
		FVector2D localPos = GetLocationPos(Geometry, _lastMovePos);
		return OnMouseButtonDownEventEx.Execute(localPos, MouseEvent.GetCursorDelta()).NativeReply;
	}
	return  Super::HandleMouseButtonDown(Geometry,MouseEvent);
}

FReply UAlphaTestParent::HandleMouseButtonUp_AlphaTest(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	ReleasePress();
	return  Super::HandleMouseButtonUp(Geometry,MouseEvent);
}

FReply UAlphaTestParent::HandleMouseDoubleClick_AlphaTest(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	ReleasePress();
	return  Super::HandleMouseDoubleClick(Geometry,MouseEvent);
}

void UAlphaTestParent::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	MyBorder->SetOnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonDown_AlphaTest));
	MyBorder->SetOnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonUp_AlphaTest));
	MyBorder->SetOnMouseMove(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseMove_AlphaTest));
	MyBorder->SetOnMouseDoubleClick(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseDoubleClick_AlphaTest));

	SAlphaTestParent* PAlphaTestParent = (SAlphaTestParent*)(MyBorder.Get());
	if(PAlphaTestParent)
	{
		PAlphaTestParent->SetOnMouseWheel(BIND_UOBJECT_DELEGATE(FOnDragDetected, HandleMouseWheel));
	}
}

FVector2D UAlphaTestParent::GetLocationPos(const FGeometry& Geometry, FVector2D screenPos)
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

FReply UAlphaTestParent::HandleMouseWheel(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if(OnMouseWheelEvent.IsBound())
	{
		OnMouseWheelEvent.Execute(MouseEvent.GetScreenSpacePosition(), MouseEvent.GetGestureDelta());
		return  FReply::Handled();
	}
	return FReply::Unhandled();
}