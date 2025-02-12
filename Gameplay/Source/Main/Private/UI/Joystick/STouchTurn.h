#pragma once

#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"

class UTouchTurn;
class MAIN_API STouchTurn : public SLeafWidget
{
public:
	STouchTurn();
	SLATE_BEGIN_ARGS(STouchTurn)
	{}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);
public:
	virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override { return 0; };
	void SetTouchTurnUmg(UTouchTurn* pUmg){m_pTouchTurnUmg = pUmg;}
private:
	void CaluTouchInter(const FVector2D& allSize);
	float Ease( float current,float duration, float changeValue);
private:
	int _CapturedPointerIndex;
	FVector2D _touchStartPos;
	FVector2D _touchMovePos;
	bool _sendTouchOneMore;
	UTouchTurn* m_pTouchTurnUmg;
	
	FPointerEventHandler OnMouseButtonDownEvent;
	FPointerEventHandler OnMouseButtonUpEvent;
public:
	float _fPitchScale;
	float _fYawScale;
	float InitValue;
	float SlopeValue;
	float InputPitchScale;
	float SpeedUpLimitPixel;
	bool bIgnoreTextureSize;

	void SetOnMouseButtonDownEvent(FPointerEventHandler EventHandler);
	void SetOnMouseButtonUpEvent(FPointerEventHandler EventHandler);
};
