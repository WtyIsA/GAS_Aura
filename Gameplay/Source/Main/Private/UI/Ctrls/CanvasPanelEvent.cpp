#include "CanvasPanelEvent.h"


#include "LuaManger.h"
#include "Input/ShortcutkeyMgr.h"
#include "SCanvasPanelEvent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "UI/UserWidgetEx/DragDropWidget.h"

UCanvasPanelEvent::UCanvasPanelEvent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	_bMouseButtonDown = false;
	bCanDrop = false;
}

void UCanvasPanelEvent::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (MyCanvas.IsValid())
	{
		
		if(_handleMouseButtonUp || _handleMouseButtonUpNotNeedDown)
		{
			MyCanvas->SetOnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonUp));
			MyCanvas->SetOnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonDownEx));
		}else if(_handleMouseButtonDown || _handleMouseRightButtonDown)
		{
			MyCanvas->SetOnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonDownEx));
		}
		if(_handleMouseMove)
		{
			MyCanvas->SetOnMouseEnter(BIND_UOBJECT_DELEGATE(FNoReplyPointerEventHandler, HandleMouseEnter));
			MyCanvas->SetOnMouseMove(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseMove));
			MyCanvas->SetOnMouseLeave(BIND_UOBJECT_DELEGATE(FSimpleNoReplyPointerEventHandler, HandleMouseLeave));
		}
	}
}

FReply UCanvasPanelEvent::HandleMouseMove(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if(_bMouseMoveShuoldDown)
	{
		if(!_bMouseButtonDown)
			return  FReply::Unhandled();
	}
	if(OnMouseMoveEvent.IsBound())
	{
		return  OnMouseMoveEvent.Execute(MouseEvent.GetScreenSpacePosition(), MouseEvent.GetCursorDelta()).NativeReply;
	}
	return  FReply::Unhandled();
}

void UCanvasPanelEvent::HandleMouseLeave(const FPointerEvent& MouseEvent)
{
	_bMouseButtonDown = false;
	if(OnMouseLeaveEvent.IsBound())
	{
		OnMouseLeaveEvent.Execute(MouseEvent.GetScreenSpacePosition(), MouseEvent.GetCursorDelta());
	}
}

void UCanvasPanelEvent::HandleMouseEnter(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if(OnMouseHover.IsBound())
		OnMouseHover.Execute(MouseEvent.GetScreenSpacePosition(), MouseEvent.GetCursorDelta());
}


FReply UCanvasPanelEvent::HandleMouseButtonDownEx(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	_bMouseButtonDown = true;
	if(_handleMouseRightButtonDown)
	{
		if(MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			if(OnMouseRightButtonDownEvent.IsBound())
				return 	OnMouseRightButtonDownEvent.Execute(MouseEvent.GetScreenSpacePosition(), MouseEvent.GetCursorDelta()).NativeReply;
		}
	}
	if(_handleMouseButtonDown)
	{
		if(OnMouseButtonDownEvent.IsBound())
			return  OnMouseButtonDownEvent.Execute(MouseEvent.GetScreenSpacePosition(), MouseEvent.GetCursorDelta()).NativeReply;
	}
	return  FReply::Unhandled();
}


FReply UCanvasPanelEvent::HandleMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if(!_bMouseButtonDown && !_handleMouseButtonUpNotNeedDown)
	{
		return  FReply::Unhandled();
	}
	_bMouseButtonDown = false;
	if(OnMouseButtonUpEvent.IsBound())
	{
		return OnMouseButtonUpEvent.Execute(MouseEvent.GetScreenSpacePosition(), MouseEvent.GetCursorDelta()).NativeReply;
	}
	return  FReply::Unhandled();
}

bool UCanvasPanelEvent::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                   UDragDropOperation* InOperation)
{
	if(!bCanDrop)
		return  false;
	//Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	UWidget * pDragWidget = Cast<UWidget>(InOperation->Payload);
	if(!IsValid(pDragWidget) || pDragWidget == this)
		return false;
	if(pDragWidget->GetVisibility() != ESlateVisibility::SelfHitTestInvisible)
		pDragWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	UDragDropWidget * pDragDropWidget = Cast<UDragDropWidget>(pDragWidget);
	if(pDragWidget != nullptr)
	{
		pDragDropWidget->OnDragSuccess();
	}
	UObject* pDrag = pDragWidget;
	FObjectProperty* pDragObj = CastField<FObjectProperty>( pDragWidget->GetClass()->FindPropertyByName(FName("_DragObject")));
	if(pDragObj != nullptr)
	{
		pDrag = pDragObj->GetPropertyValue_InContainer(pDragWidget);
	}
	if(pDrag == nullptr)
		pDrag = pDragWidget;
	int32 nType = 0;
	FIntProperty* _typeObj = CastField<FIntProperty>( pDragWidget->GetClass()->FindPropertyByName(FName("_DragType")));
	if(_typeObj)
	{
		nType = _typeObj->GetPropertyValue_InContainer(pDragWidget);
	}
	ULuaManger::Get()->DragWidget(pDrag, nType,_DropType,  this,(UUserWidget*) ULuaManger::Get()->GetTopWidget(this));
	return true;
}
/*
void UCanvasPanelEvent::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
    UDragDropOperation*& OutOperation)
{
	if(!bCanDrag)
	{
		OutOperation = nullptr;
		return;
	}
	UDragDropOperation* temp = UWidgetBlueprintLibrary::CreateDragDropOperation(nullptr);
	temp->Payload = this;
	temp->DefaultDragVisual = this;
	if(bCanDrop)
		SetVisibility(ESlateVisibility::HitTestInvisible);
	temp->Pivot = EDragPivot::MouseDown;
	OutOperation = temp;
	//Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
    
}

void UCanvasPanelEvent::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if(bCanDrop)
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	//Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}
*/
TSharedRef<SWidget> UCanvasPanelEvent::RebuildWidget()
{
	MyCanvas = SNew(SCanvasPanelEvent);

	for ( UPanelSlot* PanelSlot : Slots )
	{
		if ( UCanvasPanelSlot* TypedSlot = Cast<UCanvasPanelSlot>(PanelSlot) )
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MyCanvas.ToSharedRef());
		}
	}

	SCanvasPanelEvent* pCanvas = (SCanvasPanelEvent*)MyCanvas.Get();
	pCanvas->m_pWidget = this;
	
	return MyCanvas.ToSharedRef();
}

void UCanvasPanelEvent::NativeOnFocusLost()
{
	if(OnFocusLost.IsBound())
		OnFocusLost.Execute();
}
