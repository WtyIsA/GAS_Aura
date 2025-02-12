#include "GridPanelEx.h"

#include "LuaManger.h"
#include "SGridPanelEx.h"
#include "Components/GridSlot.h"


UGridPanelEx::UGridPanelEx(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//AutoAddChild();
	_bMouseButtonDown = false;
}

TSharedRef<SWidget> UGridPanelEx::RebuildWidget()
{
	MyGridPanel = SNew(SGridPanelEx);

	for ( UPanelSlot* PanelSlot : Slots )
	{
		if ( UGridSlot* TypedSlot = Cast<UGridSlot>(PanelSlot) )
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot( MyGridPanel.ToSharedRef() );
		}
	}

	if(handleMouse)
	{
		MyGridPanel->SetOnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonUp));
		MyGridPanel->SetOnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonDownEx));
		MyGridPanel->SetOnMouseMove(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseMove));
		MyGridPanel->SetOnMouseLeave(BIND_UOBJECT_DELEGATE(FSimpleNoReplyPointerEventHandler, HandleMouseLeave));
	}

	return MyGridPanel.ToSharedRef();	
}

void UGridPanelEx::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	for (int i = 0; i < TestNum.Y; i++) {
    		for (int j = 0; j < TestNum.X; j++) {
    			AddItem(i * 6 + j,  j, i);
    		}
    	}
}

void UGridPanelEx::AutoAddChild()
{
	UClass* toAddClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/Evolve/UI_PC/UIComponent/UIC_dna_icon.UIC_dna_icon_C"));
	UUserWidget* pButton = CreateWidget<UUserWidget>(this, toAddClass);
	AddChildToGrid(pButton);
}

UWidget* UGridPanelEx::AddItem(int nId, int inRow, int InColumn)
{
	if(ItemClass == nullptr)
	{
		UE_LOG(GAME,Warning,TEXT("[ui]UGridPanelEx::AddItem error, itemClass is null"));
		return nullptr;
	}	
	UUserWidget* pItem = CreateWidget<UUserWidget>(this, ItemClass);
	if(pItem == nullptr)
		return  nullptr;
	AddChildToGrid(pItem, inRow, InColumn);
	UGridSlot* pSlot = Cast<UGridSlot>(pItem->Slot);
	
	if(inRow % 2 == 0)
	{
		pSlot->Nudge.X = evenRowBegin.X + InColumn* ItemInter.X;
		pSlot->Nudge.Y = evenRowBegin.Y + inRow * ItemInter.Y;
	}
	else
	{
		pSlot->Nudge.X = oddRowBegin.X + InColumn* ItemInter.X;
		pSlot->Nudge.Y = oddRowBegin.Y + inRow * ItemInter.Y;
	}
	pSlot->SetNudge(pSlot->Nudge);
	pSlot->SetRow(inRow);
	pSlot->SetColumn(InColumn);

	
	SGridPanelEx* pPanelEx = (SGridPanelEx*)(MyGridPanel.Get());
	if(pPanelEx)
		pPanelEx->SetItemInterval(pSlot->Nudge, ScrollSizeXMax, ScrollSizeYMax);
	return pItem;
}

FReply UGridPanelEx::HandleMouseMove(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	
	if(!_bMouseButtonDown)
		return  FReply::Unhandled();
	
	if(OnMouseMoveEvent.IsBound())
	{
		return  OnMouseMoveEvent.Execute(MouseEvent.GetScreenSpacePosition(), MouseEvent.GetCursorDelta()).NativeReply;
	}
	return  FReply::Unhandled();
}

void UGridPanelEx::HandleMouseLeave(const FPointerEvent& MouseEvent)
{
	_bMouseButtonDown = false;
	if(OnMouseLeaveEvent.IsBound())
	{
		OnMouseLeaveEvent.Execute(MouseEvent.GetScreenSpacePosition(), MouseEvent.GetCursorDelta());
	}
}

FReply UGridPanelEx::HandleMouseButtonDownEx(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	_bMouseButtonDown = true;
	if(OnMouseButtonDownEvent.IsBound())
		return  OnMouseButtonDownEvent.Execute(MouseEvent.GetScreenSpacePosition(), MouseEvent.GetCursorDelta()).NativeReply;
	
	return  FReply::Unhandled();
}


FReply UGridPanelEx::HandleMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if(!_bMouseButtonDown)
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