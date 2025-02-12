#include "UserWidgetMouseEvent.h"

#include "LuaManger.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Input/ShortcutkeyMgr.h"
#include "Kismet/GameplayStatics.h"

UUserWidgetMouseEvent::UUserWidgetMouseEvent(const FObjectInitializer& ObjectInitializer) : UUIPanel(ObjectInitializer)
{
	_bMouseDown = false;
}

void UUserWidgetMouseEvent::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	_bMouseDown = false;
	if ( OnMouseButtonLeaveEvent.IsBound() )
	{
		OnMouseButtonLeaveEvent.Execute(ULuaManger::Get()->GetTopWidget(this),InMouseEvent.GetScreenSpacePosition(), InMouseEvent.GetCursorDelta());
		return;
	}
	Super::NativeOnMouseLeave(InMouseEvent);
	ULuaManger::Get()->OnWigetMouseLeave(ULuaManger::Get()->GetTopWidget(this),this);
}

FReply UUserWidgetMouseEvent::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(!_bMouseDown)
		return FReply::Unhandled();
	_bMouseDown = false;
	if ( OnMouseButtonUpEvent.IsBound() )
	{
		return  OnMouseButtonUpEvent.Execute(ULuaManger::Get()->GetTopWidget(this),InMouseEvent.GetScreenSpacePosition(), InMouseEvent.GetCursorDelta()).NativeReply;
	}
	bool bLuaReply = ULuaManger::Get()->OnWigetMouseButtonUp(ULuaManger::Get()->GetTopWidget(this),this, InMouseEvent.GetScreenSpacePosition());
	if (bLuaReply)
		return FReply::Handled();
	return  Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UUserWidgetMouseEvent::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{		
	FReply onReply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	if(!_bMouseDown)
		return onReply;
	if ( OnMouseButtonMoveEvent.IsBound() )
	{
		return  OnMouseButtonMoveEvent.Execute(ULuaManger::Get()->GetTopWidget(this),InMouseEvent.GetScreenSpacePosition(), InMouseEvent.GetCursorDelta()).NativeReply;
	}
	bool bLuaReply = ULuaManger::Get()->OnWigetMouseButtonMove(ULuaManger::Get()->GetTopWidget(this), this,InMouseEvent.GetScreenSpacePosition(), InMouseEvent.GetCursorDelta());
	if (bLuaReply)
		return FReply::Handled();
	else
		return onReply;
}

FReply UUserWidgetMouseEvent::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	_bMouseDown = true;
	if ( OnMouseButtonDownEvent.IsBound() )
	{
		FVector2D localPos = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
		return  OnMouseButtonDownEvent.Execute(ULuaManger::Get()->GetTopWidget(this),localPos, InMouseEvent.GetCursorDelta()).NativeReply;
	}
	FReply onReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	bool bLuaReply = ULuaManger::Get()->OnWigetMouseButtonDown(ULuaManger::Get()->GetTopWidget(this),this, InMouseEvent.GetScreenSpacePosition());
	if(bLuaReply)
		return FReply::Handled();
	else
		return onReply;
}

FReply UUserWidgetMouseEvent::NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(_SyncInputTouch)
		UShortcutkeyMgr::Get()->UpdateIntput(InMouseEvent.GetPointerIndex(), ETouchType::Began, InMouseEvent.GetScreenSpacePosition(), InMouseEvent.GetTouchForce(), FDateTime::Now(), InMouseEvent.GetTouchpadIndex() );
	_bMouseDown = true;
	if ( OnMouseButtonDownEvent.IsBound() )
	{
		return  OnMouseButtonDownEvent.Execute(ULuaManger::Get()->GetTopWidget(this),InMouseEvent.GetScreenSpacePosition(), InMouseEvent.GetCursorDelta()).NativeReply;
	}
	FReply onReply = Super::NativeOnTouchStarted(InGeometry, InMouseEvent);
	bool bLuaReply = ULuaManger::Get()->OnWigetMouseButtonDown(ULuaManger::Get()->GetTopWidget(this),this, InMouseEvent.GetScreenSpacePosition());
	if(bLuaReply)
		return FReply::Handled();
	else
		return onReply;
}

FReply UUserWidgetMouseEvent::NativeOnTouchMoved(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(_SyncInputTouch)
		UShortcutkeyMgr::Get()->UpdateIntput(InMouseEvent.GetPointerIndex(), ETouchType::Moved, InMouseEvent.GetScreenSpacePosition(), InMouseEvent.GetTouchForce(), FDateTime::Now(), InMouseEvent.GetTouchpadIndex() );
	FReply onReply = Super::NativeOnTouchMoved(InGeometry, InMouseEvent);
	if(!_bMouseDown)
		return onReply;
	if ( OnMouseButtonMoveEvent.IsBound() )
	{
		return  OnMouseButtonMoveEvent.Execute(ULuaManger::Get()->GetTopWidget(this),InMouseEvent.GetScreenSpacePosition(), InMouseEvent.GetCursorDelta()).NativeReply;
	}
	bool bLuaReply = ULuaManger::Get()->OnWigetMouseButtonMove(ULuaManger::Get()->GetTopWidget(this), this,InMouseEvent.GetScreenSpacePosition(), InMouseEvent.GetCursorDelta());
	if (bLuaReply)
		return FReply::Handled();
	else
		return onReply;
}

FReply UUserWidgetMouseEvent::NativeOnTouchEnded(const FGeometry& InGeometry, const FPointerEvent& InGestureEvent)
{
	if(_SyncInputTouch)
		UShortcutkeyMgr::Get()->UpdateIntput(InGestureEvent.GetPointerIndex(), ETouchType::Ended, InGestureEvent.GetScreenSpacePosition(), InGestureEvent.GetTouchForce(), FDateTime::Now(), InGestureEvent.GetTouchpadIndex() );
	if(!_bMouseDown)
		return FReply::Unhandled();
	_bMouseDown = false;
	if ( OnMouseButtonUpEvent.IsBound() )
	{
		return  OnMouseButtonUpEvent.Execute(ULuaManger::Get()->GetTopWidget(this),InGestureEvent.GetScreenSpacePosition(), InGestureEvent.GetCursorDelta()).NativeReply;
	}
	bool bLuaReply = ULuaManger::Get()->OnWigetMouseButtonUp(ULuaManger::Get()->GetTopWidget(this),this, InGestureEvent.GetScreenSpacePosition());
	if (bLuaReply)
		return FReply::Handled();
	return  Super::NativeOnTouchEnded(InGeometry, InGestureEvent);
}

void UUserWidgetMouseEvent::NativeOnFocusLost( const FFocusEvent& InFocusEvent )
{
	if(OnLostFocusEvent.IsBound())
	{
		OnLostFocusEvent.Execute(this, FVector2D::ZeroVector,FVector2D::ZeroVector);
		//return;
	}
	Super::NativeOnFocusLost(InFocusEvent);
}