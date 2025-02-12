#pragma once
#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"


#include "UIPanel.h"
#include "Blueprint/DragDropOperation.h"
#include "Components/ScrollBox.h"

#include "DragDropWidget.generated.h"

class UListView;

UCLASS()
class UDragDropWidget :public UUIPanel
{
    GENERATED_BODY()
public:
    UDragDropWidget(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DragDrop")
    bool bCanDrag;    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DragDrop")
    bool bCanDrop;
    UPROPERTY()
    UObject* _DragObject;
    UPROPERTY()
    int32 _DragType;  //用于区分同一个界面不同drag
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DragDrop")
    bool bListEntry;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DragDrop")
    bool bCanTouchMove = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DragDrop")
    bool bMoveScrollBox = false;
    /*在未滑动时长按后进行拖拽处理：x毫秒*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DragDrop")
    int32 ToDragTimes=2000;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DragDrop")
    float safeZone = 25;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DragDrop")
    float angleDrag = 45;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DragDrop")
    EDragPivot   _DragPivot = EDragPivot::CenterCenter;

    DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(FEventReply, FOnUserWidgetMouseEvent, UUserWidget*, widget);
    //DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserWidgetMouseEvent, UUserWidget*, widget);
    UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
    FOnUserWidgetMouseEvent OnButtonUp;
    UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
    FOnUserWidgetMouseEvent OnButtonUpNoHandle;
    UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
    FOnUserWidgetMouseEvent OnButtonDown;
    UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
    FOnUserWidgetMouseEvent OnRightButtonDown;
    virtual void SynchronizeProperties() override;
    virtual  void OnWidgetRebuilt();
    virtual void BeginDestroy() override;
    UPROPERTY()
    UUserWidget* _CurCoverWidget;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DragDrop")
    bool bHandleHover = true;
    UPROPERTY()
    UDragDropWidget* _CurDragVisualWidget;
    DECLARE_DYNAMIC_DELEGATE_OneParam(FOnHoverChangedDelegate, bool, bHover);
    UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
    FOnHoverChangedDelegate OnHoverChanged;

    void OnDragSuccess();
private:
    virtual  bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragDetected( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation ) override;
    virtual void NativeOnDragCancelled( const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation ) override;
    virtual void NativeOnDragEnter( const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation ) override;
    virtual void NativeOnDragLeave( const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation ) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
    virtual FReply NativeOnMouseButtonUp( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent );
    virtual void NativeOnMouseLeave( const FPointerEvent& InMouseEvent );
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
    virtual FReply NativeOnMouseMove( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent );

    void ResetDefultSelectMode();
    bool _bMouseDown;
    bool _bMoveing;
    bool _bDraging;
    bool _bSetSelectMode;
    bool _bIsHoveredSet;
    int64   _nDownTick;
    int64   _nDragBeginTimes;
    FVector2D _downPos;
    UListView*  _pParentListView;
    TSharedPtr<SWidget>  _ParentWidget;
    UScrollBox*  _pParentScrollBox;
    FDelegateHandle OnDelegateHandle;
    UCommonActivatableWidget* pTopParentWidget; 
};
