
#pragma once

#include "CoreMinimal.h"
#include "MaskSlate/MaskSlateWidgetStyle.h"
#include "Widgets/SLeafWidget.h"
#include "Styling/CoreStyle.h"
#include "Materials/MaterialInterface.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Materials/MaterialInstanceDynamic.h"

class FPaintArgs;
class FSlateWindowElementList;

UENUM()
enum class EEaseMode : uint8
{
	LinearIn,
	QuadEaseIn,
	QuadEaseOut,
	CubicEaseIn,
	CubicEaseOut
};

/**
 * 
 */
class SSMaskWidget : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SSMaskWidget)
		: _Style(&FCoreStyle::Get().GetWidgetStyle<FMaskSlateStyle>("SMaskWidget"))
		, _BgColorAndOpacity(FLinearColor::White)
	{
		_Visibility = EVisibility::SelfHitTestInvisible;
	}

		SLATE_STYLE_ARGUMENT(FMaskSlateStyle, Style)

		SLATE_ATTRIBUTE(FSlateColor, BgColorAndOpacity)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual bool IsInteractable() const override;

	void SetBgColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity);

	void SetBgColorAndOpacity(FLinearColor InColorAndOpacity);

	/** See attribute Style */
	void SetStyle(const FMaskSlateStyle* InStyle);

	/** Set ClickClip's MaskPosition */
	void SetMaskPosition(const int32& ClipIndex, TAttribute<FVector2D> InMaskPosition);

	/** See attribute BackgroundImage */
	void SetBackgroundImage(const FSlateBrush* InBackgroundImage);

	void ReIndexClip();

private:

	const FSlateBrush* GetBackgroundImage() const;

	const UTexture2D* GetMaskTextureByIndex(const int32& Index) const;

	const FSlateBrush* GetMaskMatBrush() const;

public:

	bool IsMaskUpdated = true;

protected:
	TAttribute<FSlateColor> BgColorAndOpacity;

	const FSlateBrush* BackgroundImage;

private:

	FMaskSlateStyle* Style;
};
