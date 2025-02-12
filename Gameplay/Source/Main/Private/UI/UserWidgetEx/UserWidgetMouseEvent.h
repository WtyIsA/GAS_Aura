#pragma once
#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "UIPanel.h"

#include "UserWidgetMouseEvent.generated.h"

UCLASS()
class UUserWidgetMouseEvent :public UUIPanel
{
    GENERATED_BODY()
public:
    UUserWidgetMouseEvent(const FObjectInitializer& ObjectInitializer);

    //DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserWidgetMouseEvent, UUserWidget*, widget);
    DECLARE_DYNAMIC_DELEGATE_RetVal_ThreeParams(FEventReply, FOnUserWidgetMouseEvent, UWidget*, widget, FVector2D, screenPos, FVector2D, CursorDelta);

    UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
    FOnUserWidgetMouseEvent OnMouseButtonDownEvent;    
    UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
    FOnUserWidgetMouseEvent OnMouseButtonUpEvent;
    /* 鼠标按下时的移动事件*/
    UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
    FOnUserWidgetMouseEvent OnMouseButtonMoveEvent;
    UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
    FOnUserWidgetMouseEvent OnMouseButtonLeaveEvent;
    UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
    FOnUserWidgetMouseEvent OnLostFocusEvent;

    /*
    * 手机端同步触摸位置
    */
    UPROPERTY(EditAnywhere,  Category=MouseEvent)
    bool _SyncInputTouch= true;
private:
    virtual void NativeOnMouseLeave( const FPointerEvent& InMouseEvent );
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
    virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
    virtual FReply NativeOnTouchStarted( const FGeometry& InGeometry, const FPointerEvent& InGestureEvent );
    virtual FReply NativeOnTouchMoved( const FGeometry& InGeometry, const FPointerEvent& InGestureEvent );
    virtual FReply NativeOnTouchEnded( const FGeometry& InGeometry, const FPointerEvent& InGestureEvent );
    virtual void NativeOnFocusLost( const FFocusEvent& InFocusEvent );
    bool _bMouseDown;
};