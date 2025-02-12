

#include "SSMaskWidget.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSMaskWidget::Construct(const FArguments& InArgs)
{
	check(InArgs._Style);
	BgColorAndOpacity = InArgs._BgColorAndOpacity;
	Style = const_cast<FMaskSlateStyle*>(InArgs._Style);

	SetCanTick(false);
}

void SSMaskWidget::SetBgColorAndOpacity(const TAttribute<FSlateColor>& InColorAndOpacity)
{
	SetAttribute(BgColorAndOpacity, InColorAndOpacity, EInvalidateWidgetReason::Paint);
}

void SSMaskWidget::SetBgColorAndOpacity(FLinearColor InColorAndOpacity)
{
	SetBgColorAndOpacity(TAttribute<FSlateColor>(InColorAndOpacity));
}

void SSMaskWidget::SetStyle(const FMaskSlateStyle* InStyle)
{
	if (InStyle == nullptr)
	{
		FArguments Defaults;
		Style = const_cast<FMaskSlateStyle*>(Defaults._Style);
	}
	else
	{
		Style = const_cast<FMaskSlateStyle*>(InStyle);
	}

	check(Style);

	Invalidate(EInvalidateWidget::Layout);

	IsMaskUpdated = true;
}

void SSMaskWidget::SetMaskPosition(const int32& ClipIndex, TAttribute<FVector2D> InMaskPosition)
{
	if (Style->SetMaskPos(ClipIndex, InMaskPosition.Get()))
	{
		IsMaskUpdated = true;
		Invalidate(EInvalidateWidget::Layout);
	}
}

void SSMaskWidget::SetBackgroundImage(const FSlateBrush* InBackgroundImage)
{
	if (BackgroundImage != InBackgroundImage)
	{
		IsMaskUpdated = true;
		BackgroundImage = InBackgroundImage;
		Invalidate(EInvalidateWidget::Layout);
	}
}

const FSlateBrush* SSMaskWidget::GetBackgroundImage() const
{
	return BackgroundImage ? BackgroundImage : &Style->BackgroundImage;
}

const UTexture2D* SSMaskWidget::GetMaskTextureByIndex(const int32& Index) const
{
	return Style->GetMaskTextureByIdx(Index);
}

const FSlateBrush* SSMaskWidget::GetMaskMatBrush() const
{
	return &Style->MaskMatBrush;
}

void SSMaskWidget::ReIndexClip()
{
	Style->ReIndexClip();
}

bool SSMaskWidget::IsInteractable() const
{
	return IsEnabled();
}

int32 SSMaskWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 RetLayerId = LayerId;
	SSMaskWidget* MutableThis = const_cast<SSMaskWidget*>(this);

	const FSlateBrush* CurBgImage = GetBackgroundImage();
	const FSlateBrush* MatBrush = GetMaskMatBrush();

#if !WITH_EDITOR
	if (MutableThis->IsMaskUpdated)
	{
#endif
		if (UMaterialInstanceDynamic* DyMat = Cast<UMaterialInstanceDynamic>(MatBrush->GetResourceObject()))
		{
			FVector2D GSize = AllottedGeometry.GetLocalSize();
			const TArray<FMaskClip> Clips = Style->MaskClips;
			for (uint8 i = 0; i < MAX_MASK_CLIP_COUNT; i++)
			{
				if (i < Clips.Num())
				{
					FMaskClip Clip = Clips[i];
					FVector2D Size = Clip.GetSize();
					FVector2D Pos = Clip.GetPos();
					DyMat->SetVectorParameterValue(*FString::Printf(TEXT("MaskUV_%d"), i), FLinearColor(Pos.X / GSize.X, Pos.Y / GSize.Y, Size.X / GSize.X, Size.Y / GSize.Y));
					if (UTexture2D* Tex = Clip.GetMaskTexture())
					{
						DyMat->SetTextureParameterValue(*FString::Printf(TEXT("MaskTex_%d"), i), Tex);
					}
				}
				else
				{
					DyMat->SetVectorParameterValue(*FString::Printf(TEXT("MaskUV_%d"), i), FLinearColor(0.f, 0.f, 0.f, 0.f));
				}
			}
			DyMat->SetTextureParameterValue("BgTex", Cast<UTexture>(CurBgImage->GetResourceObject()));
		}

#if !WITH_EDITOR
		MutableThis->IsMaskUpdated = false;
	}
#endif

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		RetLayerId++,
		AllottedGeometry.ToPaintGeometry(),
		MatBrush,
		ESlateDrawEffect::None,
		InWidgetStyle.GetColorAndOpacityTint() *
		BgColorAndOpacity.Get().GetColor(InWidgetStyle) * CurBgImage->GetTint(InWidgetStyle)
	);

	FSlateLayoutTransform TranLayout(AllottedGeometry.Scale, (FVector2D)AllottedGeometry.AbsolutePosition);
	const TArray<FMaskClip> Clips = Style->MaskClips;
	for (uint8 i = 0; i < MAX_MASK_CLIP_COUNT && i < Clips.Num(); i++)
	{
		if (Clips[i].IsEnable())
		{
			FGeometry MaskGeometry = AllottedGeometry.MakeChild(Clips[i].GetPos(), Clips[i].GetSize(), 1.f);
		}
	}

	return RetLayerId;
}

FVector2D SSMaskWidget::ComputeDesiredSize(float) const
{
	return GetBackgroundImage()->ImageSize;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
