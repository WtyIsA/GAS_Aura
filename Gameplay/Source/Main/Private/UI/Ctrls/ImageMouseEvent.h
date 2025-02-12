#pragma once
#include "Components/Image.h"

#include "ImageMouseEvent.generated.h"

UCLASS()
class UImageMouseEvent : public  UImage
{
	GENERATED_UCLASS_BODY()
public:
	virtual void SynchronizeProperties() override;

	/* 响应鼠标移动是否需要同时按下鼠标*/
	UPROPERTY(EditAnywhere,  Category=MouseEvent)
	bool _bMouseMoveShuoldDown = true;
	UPROPERTY(EditAnywhere,  Category=MouseEvent)
	bool _handleMouseMove = true;
	UPROPERTY(EditAnywhere,  Category=MouseEvent)
	bool _handleMouseButtonUp = true;
	UPROPERTY(EditAnywhere,  Category=MouseEvent)
	bool _handleMouseButtonUpNotNeedDown = false;
	

	DECLARE_DYNAMIC_DELEGATE_RetVal_ThreeParams(FEventReply, FOnMouseEvent,  FVector2D, screenPos, FVector2D, CursorDelta, int32, buttonIndex);
	
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseButtonUpEvent;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseButtonDownEventEx;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseWheelEvent;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseMoveEvent;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))	
	FOnMouseEvent OnMouseLeaveEvent;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))	
	FOnMouseEvent OnMouseEnterEvent;

	/*
	 * 手机端同步触摸位置
	 */
	UPROPERTY(EditAnywhere,  Category=MouseEvent)
	bool _SyncInputTouch= false;

	UFUNCTION()
	void DrawLine(bool bDraw, FVector2D pos1 = FVector2D::ZeroVector,FVector2D pos2 = FVector2D::ZeroVector, float fInter=0,float nPerDis = 1, float Thickness =1,  FLinearColor  color = FLinearColor::White);

	UFUNCTION()
	void ReleasePress();
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
private:
	FReply HandleMouseButtonDownEx(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	FReply HandleMouseMove(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	FReply HandleMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	void HandleMouseEnter(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	void HandleMouseLeave(const FPointerEvent& MouseEvent);
	FReply HandleMouseWheel(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	FVector2D GetLocationPos(const FGeometry& Geometry, FVector2D screenPos);
	int GetEffectButtonIndex(const FPointerEvent& MouseEvent);
	int  _nEffectButtonIndex;
	FVector2D	_lastMovePos;
};
