#pragma once
#include "Components/CanvasPanel.h"

#include "CanvasPanelEvent.generated.h"

UCLASS()
class UCanvasPanelEvent : public  UCanvasPanel
{
	GENERATED_UCLASS_BODY()
public:
	virtual void SynchronizeProperties() override;

	/* 响应鼠标移动是否需要同时按下鼠标*/
	UPROPERTY(EditAnywhere,  Category=MouseEvent)
	bool _bMouseMoveShuoldDown = true;
	UPROPERTY(EditAnywhere,  Category=MouseEvent)
	bool _handleMouseMove = false;
	UPROPERTY(EditAnywhere,  Category=MouseEvent)
	bool _handleMouseButtonUpNotNeedDown = false;
	UPROPERTY(EditAnywhere,  Category=MouseEvent)
	bool _handleMouseButtonUp = false;
	UPROPERTY(EditAnywhere,  Category=MouseEvent)
	bool _handleMouseButtonDown = false;
	UPROPERTY(EditAnywhere,  Category=MouseEvent)
	bool _handleMouseRightButtonDown = false;

	DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(FEventReply, FOnMouseEvent,  FVector2D, screenPos, FVector2D, CursorDelta);

	DECLARE_DYNAMIC_DELEGATE( FOnFocusLost);

	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseButtonDownEvent;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseRightButtonDownEvent;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseButtonUpEvent;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseHover;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseMoveEvent;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))	
	FOnMouseEvent OnMouseLeaveEvent;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))	
	FOnFocusLost OnFocusLost;

	   
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DragDrop")
	bool bCanDrop = false;	
	UPROPERTY()
	int32 _DropType = 0;  //用于区分同一个界面不同drag
private:
	FReply HandleMouseButtonDownEx(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	FReply HandleMouseMove(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	FReply HandleMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	void HandleMouseLeave(const FPointerEvent& MouseEvent);
	void HandleMouseEnter(const FGeometry& Geometry, const FPointerEvent& MouseEvent);

	bool _bMouseButtonDown;
public:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) ;
	void NativeOnFocusLost();
	//virtual void NativeOnDragDetected( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation ) ;
	//virtual void NativeOnDragCancelled( const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation ) ;
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
};
