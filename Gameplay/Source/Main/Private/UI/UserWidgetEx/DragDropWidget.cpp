#include "DragDropWidget.h"
#include "LuaManger.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Framework/Application/SlateUser.h"

UDragDropWidget::UDragDropWidget(const FObjectInitializer& ObjectInitializer) : UUIPanel(ObjectInitializer)
{
    bCanDrag = false;
    bCanDrop = false;
    _DragObject = nullptr;
    _DragType = 0;
    bListEntry = true;
    _bMouseDown = false;
    _nDownTick = 0;
    _nDragBeginTimes = ETimespan::TicksPerMillisecond * ToDragTimes;
    _bDraging = _bMoveing = false;
    _pParentListView = NULL;
    _pParentScrollBox = NULL;
    _bIsHoveredSet = false;
    _CurDragVisualWidget = nullptr;
    pTopParentWidget = nullptr;
}

bool UDragDropWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                   UDragDropOperation* InOperation)
{
    _CurCoverWidget = nullptr;
    if (_CurDragVisualWidget)
    {
        _CurDragVisualWidget->RemoveFromParent();
        _CurDragVisualWidget = nullptr;
    }
    if (UDragDropWidget* payload = Cast<UDragDropWidget>(InOperation->Payload))
    {
        payload->_CurCoverWidget = nullptr;
        if(!payload->_CurDragVisualWidget && payload != this)
        {
            return  true;
        }
        if (payload->_CurDragVisualWidget)
        {            
            payload->_CurDragVisualWidget->RemoveFromParent();
            payload->_CurDragVisualWidget = nullptr;
        }
    }
    if(!bCanDrop)
        return  false;
    ResetDefultSelectMode();
    Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
    UDragDropWidget * pDragWidget = Cast<UDragDropWidget>(InOperation->Payload);
    if(!IsValid(pDragWidget))
        return false;
    //if(pDragWidget->GetVisibility() != ESlateVisibility::SelfHitTestInvisible)
    //    pDragWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    pDragWidget->OnDragSuccess();
    UObject* pDrag = pDragWidget->_DragObject? pDragWidget->_DragObject:pDragWidget;
    ULuaManger::Get()->DragWidget(pDrag, pDragWidget->_DragType,_DragType,  this,(UUserWidget*) ULuaManger::Get()->GetTopWidget(this));
    return true;
}

void UDragDropWidget::NativeOnDragEnter( const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation )
{
    if (!bHandleHover)
        return;
    UDragDropWidget * FromWidget = Cast<UDragDropWidget>(InOperation->Payload);
    //if (FromWidget->_CurCoverWidget != this)
    //{
        if(IsValid(this))
        {
            FromWidget->_CurCoverWidget = this;
            ULuaManger::Get()->OnCoverWidgetChange((UUserWidget*) ULuaManger::Get()->GetTopWidget(FromWidget), (UUserWidget*) ULuaManger::Get()->GetTopWidget(this),this, FromWidget, FromWidget->_DragType, _DragType);
        }
    //}
}

void UDragDropWidget::NativeOnDragLeave( const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation )
{
    if (!bHandleHover)
        return;
    if (UDragDropWidget* FromWidget = Cast<UDragDropWidget>(InOperation->Payload))
    {
        if (FromWidget->_CurCoverWidget == this)
        {
            FromWidget->_CurCoverWidget = nullptr;
            ULuaManger::Get()->OnCoverWidgetChange((UUserWidget*) ULuaManger::Get()->GetTopWidget(FromWidget), nullptr, nullptr, FromWidget, FromWidget->_DragType, _DragType);
        }
    }
}

//FUMGDragDropOp
void UDragDropWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
    UDragDropOperation*& OutOperation)
{
    if(!bCanDrag || _bMoveing)
    {
        OutOperation = nullptr;
        return;
    }
    _bDraging = true;
    FVector2D offset = FVector2D::ZeroVector;
    if(_DragPivot == EDragPivot::MouseDown)
    {
        _DragPivot = EDragPivot::TopLeft;        
    }
    UDragDropOperation* temp = UWidgetBlueprintLibrary::CreateDragDropOperation(nullptr);
    temp->Payload = this;
    //temp->DefaultDragVisual = this;
    temp->Offset = offset;
    //if(bCanDrop)
    //    SetVisibility(ESlateVisibility::HitTestInvisible);
    temp->Pivot = _DragPivot;
    if (!_CurDragVisualWidget)
    {
        _CurDragVisualWidget = Cast<UDragDropWidget>(StaticDuplicateObject(this,ULuaManger::Get()->GetTopWidget(this)));
    }
    _CurDragVisualWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
    // UWidget* parent = GetParent();
    // int loopCount = 0;
    // while (parent)
    // {
    //     loopCount++;
    //     if (UCanvasPanel* cp = Cast<UCanvasPanel>(parent))
    //     {
    //         cp->AddChildToCanvas(_CurDragVisualWidget);
    //         break;
    //     }
    //     parent = GetParent();
    //     if (loopCount > 100)
    //     {
    //         FString moduleName = TEXT("UDragDropWidget");
    //         FString logContent = TEXT("Not find parent for DragVisualWidget.");
    //         ULuaManger::Get()->Log(ELogVerbosity::Warning,moduleName,logContent);
    //         break;
    //     }
    // }
    temp->DefaultDragVisual = _CurDragVisualWidget;
    OutOperation = temp;
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
    UObject* pDrag = _DragObject? _DragObject:this;
    ULuaManger::Get()->OnBeginDragWidget(pDrag, _DragType, (UUserWidget*) ULuaManger::Get()->GetTopWidget(this), _CurDragVisualWidget);
}

void UDragDropWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    ResetDefultSelectMode();
    if (bCanDrag && _ParentWidget.IsValid())
    {
        TakeWidget()->AssignParentWidget(_ParentWidget);
        _ParentWidget.Reset();
    }
    //if(bCanDrop)
    //    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
    UObject* pDrag = _DragObject? _DragObject:this;
    ULuaManger::Get()->OnCancelDragWidget(pDrag, _DragType,  (UUserWidget*) ULuaManger::Get()->GetTopWidget(this));
    _CurCoverWidget = nullptr;
    if (_CurDragVisualWidget)
    {
        _CurDragVisualWidget->RemoveFromParent();
        _CurDragVisualWidget = nullptr;
    }

    if (UDragDropWidget* payload = Cast<UDragDropWidget>(InOperation->Payload))
    {
        payload->_CurCoverWidget = nullptr;
        if (payload->_CurDragVisualWidget)
        {
            payload->_CurDragVisualWidget->RemoveFromParent();
            payload->_CurDragVisualWidget = nullptr;
        }
    }
}

FReply UDragDropWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if(InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        if ( OnRightButtonDown.IsBound() )
        {
            return  OnRightButtonDown.Execute(this).NativeReply;
        }
        else
            return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);;
    }
    else
    {
        if (OnButtonDown.IsBound())
        {
            OnButtonDown.Execute(this);
        }
        _ParentWidget = TakeWidget()->GetParentWidget();
        _bMouseDown = true;
        _bMoveing = _bDraging = false;
        if (bListEntry && (bCanTouchMove || bCanDrag))
        {
            _nDownTick = FDateTime::Now().GetTicks();
            _downPos = InMouseEvent.GetScreenSpacePosition();

            Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
            return FReply::Handled().CaptureMouse(GetCachedWidget().ToSharedRef());
        }
        if (bCanDrag)
        {        
            return   UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent,this, EKeys::LeftMouseButton).NativeReply;
        }    
    }
    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);;
}

FReply UDragDropWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    _CurCoverWidget = nullptr;
    if (OnButtonUpNoHandle.IsBound())
    {
        OnButtonUpNoHandle.Execute(this);
    }
    if (_CurDragVisualWidget)
    {
        _CurDragVisualWidget->RemoveFromParent();
        _CurDragVisualWidget = nullptr;
    }
    if(_bMoveing || _bDraging)
    {
        if(_bMoveing)
        {
            if (bMoveScrollBox)
            {
                if (_pParentScrollBox)
                {
                    _pParentScrollBox->GetCachedWidget()->OnMouseButtonUp(InGeometry, InMouseEvent);
                }
            }
            else
            {
                if (_pParentListView)
                {
                    _pParentListView->GetCachedWidget()->OnTouchEnded(InGeometry, InMouseEvent);
                }
            }
        }
        _bMouseDown = _bMoveing = _bDraging = false;
        return  FReply::Handled().ReleaseMouseCapture();
    }
    if(bCanDrag || bCanTouchMove)
    {
        if (_pParentListView && InGeometry.IsUnderLocation(InMouseEvent.GetScreenSpacePosition()))
        {

            TSharedPtr<const IObjectTableRow> SlateRow = IObjectTableRow::ObjectRowFromUserWidget(Cast<UUserWidget>(this));
            //const SObjectTableRow<UObject>* pIRow = Cast<const SObjectTableRow<UObject>>(SlateRow.Get());
            TSharedPtr<SListView<UObject*>> psListView = StaticCastSharedPtr<SListView<UObject*>>(_pParentListView->GetCachedWidget());
            if(SlateRow.IsValid() && psListView.IsValid())
            {
                auto itemTemp = psListView->ItemFromWidget(SlateRow.Get());
                UObject* pTemp = Cast<UObject>(*itemTemp);
                if(_pParentListView->GetSelectionMode() == ESelectionMode::Type::Single)
                {
                    if (_pParentListView->GetSelectedItem<UObject>() != pTemp)
                    {
                        _pParentListView->SetSelectedItem(pTemp);
                    }
                }
                else if(_pParentListView->GetSelectionMode() == ESelectionMode::Type::SingleToggle)
                {
                    if (_pParentListView->GetSelectedItem<UObject>() != pTemp)
                    {
                        _pParentListView->SetSelectedItem(pTemp);
                    }
                    else
                    {
                        _pParentListView->ClearSelection();
                    }
                    psListView->Private_OnItemClicked(pTemp);
                    if (HasMouseCapture())
                    {
                        return FReply::Handled().ReleaseMouseCapture();
                    }
                    return FReply::Handled();
                }
            }
        }
        if(_bMouseDown && OnButtonUp.IsBound()  && InGeometry.IsUnderLocation(InMouseEvent.GetScreenSpacePosition()))
        {
            OnButtonUp.Execute(this);
        }

        _bMouseDown = false;
        return FReply::Unhandled().ReleaseMouseCapture();
    }
    //ResetDefultSelectMode();
    if(!_bMouseDown)
        return FReply::Unhandled();
    if ( OnButtonUp.IsBound() )
    {
        if (HasMouseCapture())
        {
            return OnButtonUp.Execute(this).NativeReply.ReleaseMouseCapture();
        }
        return  OnButtonUp.Execute(this).NativeReply;
    }
    return  Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UDragDropWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    if(!bCanTouchMove)
        _bMouseDown = false;
    Super::NativeOnMouseLeave(InMouseEvent);
    if (_bIsHoveredSet)
        OnHoverChanged.ExecuteIfBound(false);
    _bIsHoveredSet = false;
}

void UDragDropWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    if (!_bIsHoveredSet)
        OnHoverChanged.ExecuteIfBound(true);
    _bIsHoveredSet = true;
}


FReply UDragDropWidget::NativeOnMouseMove( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent )
{
    if(!_bMouseDown)
    {
        return  FReply::Unhandled();
    }
    if(!GetCachedWidget()->HasMouseCapture())
    {
        return  FReply::Unhandled();
    }
    if (bCanTouchMove)
    {        
        if(_bDraging)
            return FReply::Handled();
        
        if(!_bMoveing)
        {
            if(bCanDrag)
            {
                int64 temp  = FDateTime::Now().GetTicks();
                if(temp - _nDownTick > _nDragBeginTimes)
                {
                    _bDraging = true;
                    return FReply::Handled().DetectDrag(GetCachedWidget().ToSharedRef(), EKeys::LeftMouseButton).CaptureMouse(GetCachedWidget().ToSharedRef());
               
                };
            }
            FVector2D nowPos = InMouseEvent.GetScreenSpacePosition();
            if(nowPos !=  _downPos)
            {
                FVector2D tempDistance = nowPos - _downPos;
                if(tempDistance.SizeSquared() < safeZone*safeZone)
                    return FReply::Unhandled();
                if(bCanDrag)
                {
                    if(tempDistance.Y < 0)
                        tempDistance.Y = 0-tempDistance.Y;
                    if(tempDistance.X < 0)
                        tempDistance.X = 0-tempDistance.X;
                    float Angle = FMath::RadiansToDegrees<float>( FMath::Atan2(tempDistance.Y, tempDistance.X));
                    if (bMoveScrollBox)
                    {
                        if(_pParentScrollBox && ((_pParentScrollBox->Orientation == Orient_Vertical && Angle <= angleDrag) ||
                                                (_pParentScrollBox->Orientation == Orient_Horizontal && Angle >= angleDrag)))
                        {
                            _bDraging = true;
                            return FReply::Handled().DetectDrag(GetCachedWidget().ToSharedRef(), EKeys::LeftMouseButton).CaptureMouse(GetCachedWidget().ToSharedRef());
                        }
                    }
                    else
                    {
                        if(_pParentListView && ((_pParentListView->GetOrientation() == Orient_Vertical && Angle <= angleDrag) ||
                                                (_pParentListView->GetOrientation() == Orient_Horizontal && Angle >= angleDrag)))
                        {
                            _bDraging = true;
                            return FReply::Handled().DetectDrag(GetCachedWidget().ToSharedRef(), EKeys::LeftMouseButton).CaptureMouse(GetCachedWidget().ToSharedRef());
                        }
                    }
                }
            }
            else
            {
                return FReply::Unhandled();
            }
        }
        
        
        _bMoveing = true;
        if (bMoveScrollBox)
        {
            if(_pParentScrollBox)
                _pParentScrollBox->GetCachedWidget()->OnMouseMove(InGeometry, InMouseEvent);
        }
        else
        {
            if(_pParentListView)
                _pParentListView->GetCachedWidget()->OnTouchMoved(InGeometry, InMouseEvent);
        }
        return FReply::Unhandled();
    }
    else if (bCanDrag)
    {
        if(_bDraging)
            return FReply::Handled();
        if (safeZone > 0)
        {
            FVector2D nowPos = InMouseEvent.GetScreenSpacePosition();
            if (nowPos != _downPos)
            {
                FVector2D tempDistance = nowPos - _downPos;
                if (tempDistance.SizeSquared() < safeZone * safeZone)
                    return FReply::Unhandled();
                _bDraging = true;
                return FReply::Handled().DetectDrag(GetCachedWidget().ToSharedRef(), EKeys::LeftMouseButton).CaptureMouse(GetCachedWidget().ToSharedRef());
            }
        }
        else
        {
            _bDraging = true;
            return FReply::Handled().DetectDrag(GetCachedWidget().ToSharedRef(), EKeys::LeftMouseButton).CaptureMouse(GetCachedWidget().ToSharedRef());
        }
    }
    return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UDragDropWidget::SynchronizeProperties()
{
    Super::SynchronizeProperties();
    _nDragBeginTimes = ETimespan::TicksPerMillisecond * ToDragTimes;   
}
void UDragDropWidget::OnWidgetRebuilt()
{
    if(_pParentListView == nullptr)
    {
        _pParentListView = Cast<UListView>(UUserListEntryLibrary::GetOwningListView(this));        
    }
    if (_pParentScrollBox == nullptr)
    {
        UPanelWidget* parent;
        if (_pParentListView)
        {
            parent = _pParentListView ->GetParent();
        }
        else
        {
            parent = GetParent();
        }
        while (parent)
        {
            _pParentScrollBox = Cast<UScrollBox>(parent);
            if (_pParentScrollBox)
            {
                break;
            }
            parent = parent->GetParent();
        }
    }    
    _CurDragVisualWidget = nullptr;
    Super::OnWidgetRebuilt();    
#if !UE_EDITOR
    pTopParentWidget = Cast<UCommonActivatableWidget>(ULuaManger::Get()->GetTopWidget(this));
    if(pTopParentWidget)
    {
        OnDelegateHandle = pTopParentWidget->OnDeactivated().AddLambda([this]()
        {    
            if(_bDraging)
            {
                TSharedPtr<FSlateUser> User = FSlateApplication::Get().GetUser(0);
                if (User.IsValid())
                {
                    User->CancelDragDrop();
                }
            }
            ResetDefultSelectMode();
        });
    }
#endif    
}

void UDragDropWidget::BeginDestroy()
{
#if !UE_EDITOR
    if(pTopParentWidget)
        pTopParentWidget->OnDeactivated().Remove(OnDelegateHandle);
#endif
    Super::BeginDestroy();
}


void UDragDropWidget::ResetDefultSelectMode()
{    
    _bMoveing = false;
    _bDraging = false;
    _bMouseDown = false;
}

void UDragDropWidget::OnDragSuccess()
{
    ResetDefultSelectMode();
    if (bCanDrag && _ParentWidget.IsValid())
    {
        TakeWidget()->AssignParentWidget(_ParentWidget);
        _ParentWidget.Reset();
    }        
}
