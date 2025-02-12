#pragma once
#include "Components/GridPanel.h"
#include "CoreMinimal.h"
#include "GridPanelEx.generated.h"


USTRUCT(BlueprintType)
struct FGridPanelAutoAddChildren
{
	GENERATED_BODY()
	/*
	* 选项之间的间隔度数，360
	*/
	UPROPERTY(EditAnywhere, Category = "GridPanelAutoAddChildren")
	int Rows = 6;
	UPROPERTY(EditAnywhere, Category = "GridPanelAutoAddChildren")
	FVector2D childSize = FVector2D(300,300);
	/*响应鼠标事件的最小半径*/
	UPROPERTY(EditAnywhere, Category = "GridPanelAutoAddChildren")
	float evenRowBegin = 100;
	/*响应鼠标事件的最大半径*/
	UPROPERTY(EditAnywhere, Category = "GridPanelAutoAddChildren")
	float oddRowBegin = 150;
	/*按钮的icon离中心的距离*/
	UPROPERTY(EditAnywhere, Category = "GridPanelAutoAddChildren")
	float	  ItemInter =100;
	UPROPERTY(EditAnywhere, Category = "GridPanelAutoAddChildren")
	UClass*	  ButtonClass;
};

UCLASS()
class UGridPanelEx : public UGridPanel
{
	GENERATED_UCLASS_BODY()
public:	
	/*偶数行第一个偏移量*/
	UPROPERTY(EditAnywhere, Category = "GridPanelAutoAddChildren")
	FVector2D evenRowBegin = FVector2D(100,10);
	/*奇数行第一个偏移量*/
	UPROPERTY(EditAnywhere, Category = "GridPanelAutoAddChildren")
	FVector2D oddRowBegin = FVector2D(195, 10);
	UPROPERTY(EditAnywhere, Category = "GridPanelAutoAddChildren")
	FVector2D ItemInter = FVector2D(70,-50);
	
	UPROPERTY(EditAnywhere, Category = "GridPanelAutoAddChildren")
	UClass*	  ItemClass;

	/*水平滚轮时大小放大，false为缩小*/
	UPROPERTY(EditAnywhere, Category = "GridPanelAutoAddChildren")
	bool ScrollSizeXMax = true;
	UPROPERTY(EditAnywhere, Category = "GridPanelAutoAddChildren")
	/*垂直滚轮时大小放大，false为缩小*/
	bool ScrollSizeYMax = false;
	
	UPROPERTY(EditAnywhere, Category = "GridPanelAutoAddChildren")
	FVector2D TestNum = FVector2D(0,0);

	UFUNCTION()
	UWidget* AddItem(int nId, int inRow, int InColumn);

	DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(FEventReply, FOnMouseEvent,  FVector2D, screenPos, FVector2D, CursorDelta);
	UPROPERTY(EditAnywhere, Category=Events)
	bool handleMouse = false;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseButtonDownEvent;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseButtonUpEvent;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnMouseEvent OnMouseMoveEvent;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))	
	FOnMouseEvent OnMouseLeaveEvent;
private:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;
	void AutoAddChild();

	FReply HandleMouseButtonDownEx(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	FReply HandleMouseMove(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	FReply HandleMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	void HandleMouseLeave(const FPointerEvent& MouseEvent);
	bool _bMouseButtonDown;
};
