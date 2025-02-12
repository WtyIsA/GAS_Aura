//gjy

#pragma once

#include "CoreMinimal.h"
#include "SAlphaTestParent.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "AlphaTestParent.generated.h"

/**
 * Child must can't response mouse event. Script will set child's visibility to ESlateVisibility::HitTestInvisible.
 */
UCLASS()
class MAIN_API UAlphaTestParent : public UBorder
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void ReleasePress();
	virtual void SynchronizeProperties() override;
	FReply HandleMouseMove_AlphaTest(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	FReply HandleMouseButtonDown_AlphaTest(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	FReply HandleMouseButtonUp_AlphaTest(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	FReply HandleMouseDoubleClick_AlphaTest(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	
	DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(FEventReply, FOnMouseEvent,  FVector2D, screenPos, FVector2D, CursorDelta);
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseMoveEventEx;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseButtonDownEventEx;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseWheelEvent;
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	uint8 _bMouseButtonDown : 1;
	FVector2D _lastMovePos;

	FVector2D GetLocationPos(const FGeometry& Geometry, FVector2D screenPos);
	FReply HandleMouseWheel(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
};
