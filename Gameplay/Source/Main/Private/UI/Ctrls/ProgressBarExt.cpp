// @AmyAn.2021


#include "ProgressBarExt.h"

#include "SProgressBarExt.h"

#define LOCTEXT_NAMESPACE "UMG"

static FProgressBarStyle* DefaultProgressBarStyle = nullptr;

UProgressBarExt::UProgressBarExt(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	if (DefaultProgressBarStyle == nullptr)
	{
		// HACK: THIS SHOULD NOT COME FROM CORESTYLE AND SHOULD INSTEAD BE DEFINED BY ENGINE TEXTURES/PROJECT SETTINGS
		DefaultProgressBarStyle = new FProgressBarStyle(
			FCoreStyle::Get().GetWidgetStyle<FProgressBarStyle>("ProgressBar"));

		// Unlink UMG default colors from the editor settings colors.
		DefaultProgressBarStyle->UnlinkColors();
	}

	WidgetStyle = *DefaultProgressBarStyle;
	WidgetStyle.FillImage.TintColor = FLinearColor::White;

	BarFillType = EFillMode::LeftToRight;
	bIsMarquee = false;
	Percent = 0;
	StartAngle = 0;
	FillColorAndOpacity = FLinearColor::White;
	BorderPadding = FVector2D(0, 0);
}

void UProgressBarExt::SetPercent(float InPercent)
{
	Percent = InPercent;
	if (MyProgressBar.IsValid())
	{
		MyProgressBar->SetPercent(InPercent);
	}
}

void UProgressBarExt::SetFillColorAndOpacity(FLinearColor InColor)
{
	FillColorAndOpacity = InColor;
	if (MyProgressBar.IsValid())
	{
		MyProgressBar->SetFillColorAndOpacity(FillColorAndOpacity);
	}
}

void UProgressBarExt::SetIsMarquee(bool InbIsMarquee)
{
	bIsMarquee = InbIsMarquee;
	if (MyProgressBar.IsValid())
	{
		MyProgressBar->SetPercent(bIsMarquee ? TOptional<float>() : Percent);
	}
}

void UProgressBarExt::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	const TAttribute<TOptional<float>> PercentBinding = OPTIONAL_BINDING_CONVERT(
		float, Percent, TOptional<float>, ConvertFloatToOptionalFloat);

	const TAttribute<TOptional<float>> StartAngleBinding = OPTIONAL_BINDING_CONVERT(
		float, StartAngle, TOptional<float>, ConvertFloatToOptionalFloat);

	const TAttribute<FSlateColor> FillColorAndOpacityBinding = PROPERTY_BINDING(FSlateColor, FillColorAndOpacity);

	MyProgressBar->SetStyle(&WidgetStyle);

	MyProgressBar->SetBarFillType(BarFillType);
	MyProgressBar->SetPercent(bIsMarquee ? TOptional<float>() : PercentBinding);
	MyProgressBar->SetStartAngle(bIsMarquee ? TOptional<float>() : StartAngleBinding);
	MyProgressBar->SetFillColorAndOpacity(FillColorAndOpacityBinding);
	MyProgressBar->SetBorderPadding(BorderPadding);
}

void UProgressBarExt::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyProgressBar.Reset();
}

void UProgressBarExt::PostLoad()
{
	Super::PostLoad();
}

#if WITH_EDITOR
const FText UProgressBarExt::GetPaletteCategory()
{
	return LOCTEXT("UMGScaffold", "Scaffold!");
}

void UProgressBarExt::OnCreationFromPalette()
{
	FillColorAndOpacity = FLinearColor(0, 0.5f, 1.0f);
	Percent = 0.64f;
	StartAngle = 0.f;
}
#endif

TSharedRef<SWidget> UProgressBarExt::RebuildWidget()
{
	MyProgressBar = SNew(SProgressBarExt);
	return MyProgressBar.ToSharedRef();
}
