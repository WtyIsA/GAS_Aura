// @AmyAn.2021

#pragma once

#include "CoreMinimal.h"

#include "SProgressBarExt.h"
#include "Components/Widget.h"

#include "ProgressBarExt.generated.h"

/**
 * 
 */
UCLASS()
class MAIN_API UProgressBarExt : public UWidget
{
	GENERATED_UCLASS_BODY()
public:
	
	/** The progress bar style */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Style", meta=( DisplayName="Style" ))
	FProgressBarStyle WidgetStyle;

	/** Used to determine the fill position of the progress bar ranging 0..1 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Progress, meta=( UIMin = "0", UIMax = "1" ))
	float Percent;
	
	/** Defines if this progress bar fills Left to right or right to left */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Progress)
	TEnumAsByte<EFillMode::Type> BarFillType;
	// EFillMode BarFillType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Progress, meta=(EditCondition="BarFillType == EFillMode::ClockWise || BarFillType == EFillMode::CounterClockWise",  UIMin = "0", UIMax = "1" ))
	float StartAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Progress)
	bool bIsMarquee;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Progress)
	FVector2D BorderPadding;

	/** Fill Color and Opacity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FLinearColor FillColorAndOpacity;
	
	/** A bindable delegate to allow logic to drive the text of the widget */
	UPROPERTY()
	FGetFloat PercentDelegate;
	
	/** A bindable delegate to allow logic to drive the text of the widget */
	UPROPERTY()
	FGetFloat StartAngleDelegate;
	

	/** */
	UPROPERTY()
	FGetLinearColor FillColorAndOpacityDelegate;

	/** Sets the current value of the ProgressBar. */
	UFUNCTION(BlueprintCallable, Category="Progress")
	void SetPercent(float InPercent);

	/** Sets the fill color of the progress bar. */
	UFUNCTION(BlueprintCallable, Category="Progress")
	void SetFillColorAndOpacity(FLinearColor InColor);

	/** Sets the progress bar to show as a marquee. */
	UFUNCTION(BlueprintCallable, Category="Progress")
	void SetIsMarquee(bool InbIsMarquee);

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

	virtual void PostLoad() override;
	
#if WITH_EDITOR
	//~ Begin UWidget Interface
	virtual const FText GetPaletteCategory() override;
	virtual void OnCreationFromPalette() override;
	//~ End UWidget Interface
#endif

protected:
	/** Native Slate Widget */
	TSharedPtr<class SProgressBarExt> MyProgressBar;

	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

	PROPERTY_BINDING_IMPLEMENTATION(FSlateColor, FillColorAndOpacity);
};
