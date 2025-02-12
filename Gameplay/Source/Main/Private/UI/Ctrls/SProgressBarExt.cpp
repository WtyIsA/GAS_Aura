#include "SProgressBarExt.h"

void SProgressBarExt::Construct(const FArguments& InArgs)
{
	check(InArgs._Style);

	MarqueeOffset = 0.0f;

	Style = InArgs._Style;

	SetPercent(InArgs._Percent);
	SetStartAngle(InArgs._StartAngle);

	BarFillType = InArgs._BarFillType;

	BackgroundImage = InArgs._BackgroundImage;
	FillImage = InArgs._FillImage;
	MarqueeImage = InArgs._MarqueeImage;

	FillColorAndOpacity = InArgs._FillColorAndOpacity;
	BorderPadding = InArgs._BorderPadding;

	CurrentTickRate = 0.0f;
	MinimumTickRate = InArgs._RefreshRate;

	SetCanTick(false);

	UpdateMarqueeActiveTimer();
}

// Returns false if the clipping zone area is zero in which case we should skip drawing
bool PushTransformedClipExt(FSlateWindowElementList& OutDrawElements, const FGeometry& AllottedGeometry,
                         FVector2D InsetPadding, FVector2D ProgressOrigin, FSlateRect Progress)
{
	const FSlateRenderTransform& Transform = AllottedGeometry.GetAccumulatedRenderTransform();

	const FVector2D MaxSize = AllottedGeometry.GetLocalSize() - (InsetPadding * 2.0f);

	const FSlateClippingZone ClippingZone(
		Transform.TransformPoint(InsetPadding + (ProgressOrigin - FVector2D(Progress.Left, Progress.Top)) * MaxSize),
		Transform.TransformPoint(
			InsetPadding + FVector2D(ProgressOrigin.X + Progress.Right, ProgressOrigin.Y - Progress.Top) * MaxSize),
		Transform.TransformPoint(
			InsetPadding + FVector2D(ProgressOrigin.X - Progress.Left, ProgressOrigin.Y + Progress.Bottom) * MaxSize),
		Transform.TransformPoint(
			InsetPadding + (ProgressOrigin + FVector2D(Progress.Right, Progress.Bottom)) * MaxSize));

	if (ClippingZone.HasZeroArea())
	{
		return false;
	}

	OutDrawElements.PushClip(ClippingZone);
	return true;
}

FVector2D UnitValToUV(float Val,
                      const FVector2D UVCenter = FVector2D(.5f),
                      const FVector2D UVSize = FVector2D::UnitVector)
{
	if (Val < 0)
	{
		Val += 1;
	}
	if (Val > 1)
	{
		Val -= 1;
	}
	const float TAU = 6.28318;
	// Minimal version of Liang-Barsky clipping algorithm
	const float Angle = Val * TAU - TAU * 0.25;
	FVector2D Dir = FVector2D(FMath::Cos(Angle), FMath::Sin(Angle));
	float T1 = 1.0;
	float Cp = 0.0;
	float Cq = 0.0;

	for (int Edge = 0; Edge < 4; Edge++)
	{
		if (Edge == 0)
		{
			const float EdgeLeft = 0.0;
			if (Dir.X > 0)
			{
				continue;
			}
			Cq = -(EdgeLeft - UVCenter.X);
			Dir.X *= 2.0 * Cq;
			Cp = -Dir.X;
		}
		else if (Edge == 1)
		{
			const float EdgeRight = 1.0;
			if (Dir.X < 0)
			{
				continue;
			}
			Cq = EdgeRight - UVCenter.X;
			Dir.X *= 2.0 * Cq;
			Cp = Dir.X;
		}
		else if (Edge == 2)
		{
			const float EdgeBottom = 0.0;
			if (Dir.Y > 0)
			{
				continue;
			}
			Cq = -(EdgeBottom - UVCenter.Y);
			Dir.Y *= 2.0 * Cq;
			Cp = -Dir.Y;
		}
		else if (Edge == 3)
		{
			const float EdgeTop = 1.0;
			if (Dir.Y < 0)
			{
				continue;
			}
			Cq = EdgeTop - UVCenter.Y;
			Dir.Y *= 2.0 * Cq;
			Cp = Dir.Y;
		}
		const float Cr = Cq / Cp;
		if (Cr >= 0 && Cr < T1)
		{
			T1 = Cr;
		}
	}
	return UVCenter + T1 * Dir * UVSize;
}

int32 SProgressBarExt::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                               const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
                               const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// Used to track the layer ID we will return.
	int32 RetLayerId = LayerId;

	bool bEnabled = ShouldBeEnabled(bParentEnabled);
	const ESlateDrawEffect DrawEffects = bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
	const FSlateBrush* CurrentFillImage = GetFillImage();

	const FLinearColor FillColorAndOpacitySRGB(
		InWidgetStyle.GetColorAndOpacityTint() * FillColorAndOpacity.Get().GetColor(InWidgetStyle) * CurrentFillImage->
		GetTint(InWidgetStyle));
	const FLinearColor ColorAndOpacitySRGB = InWidgetStyle.GetColorAndOpacityTint();

	TOptional<float> ProgressFraction = Percent.Get();

	FVector2D BorderPaddingRef = BorderPadding.Get();

	const FSlateBrush* CurrentBackgroundImage = GetBackgroundImage();

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		RetLayerId++,
		AllottedGeometry.ToPaintGeometry(),
		CurrentBackgroundImage,
		DrawEffects,
		InWidgetStyle.GetColorAndOpacityTint() * CurrentBackgroundImage->GetTint(InWidgetStyle)
	);

	if (ProgressFraction.IsSet())
	{
		EFillMode::Type ComputedBarFillType = BarFillType;
		if (GSlateFlowDirection == EFlowDirection::RightToLeft)
		{
			switch (ComputedBarFillType)
			{
			case EFillMode::Type::LeftToRight:
				ComputedBarFillType = EFillMode::RightToLeft;
				break;
			case EFillMode::Type::RightToLeft:
				ComputedBarFillType = EFillMode::Type::LeftToRight;
				break;
			case EFillMode::Type::ClockWise:
				ComputedBarFillType = EFillMode::Type::CounterClockWise;
				break;
			case EFillMode::Type::CounterClockWise:
				ComputedBarFillType = EFillMode::Type::ClockWise;
				break;
			}
		}

		const float ClampedFraction = FMath::Clamp(ProgressFraction.GetValue(), 0.0f, 1.0f);
		switch (ComputedBarFillType)
		{
		case EFillMode::Type::RightToLeft:
			{
				if (PushTransformedClipExt(OutDrawElements, AllottedGeometry, BorderPaddingRef, FVector2D(1, 0),
				                        FSlateRect(ClampedFraction, 0, 0, 1)))
				{
					// Draw Fill
					FSlateDrawElement::MakeBox(
						OutDrawElements,
						RetLayerId++,
						AllottedGeometry.ToPaintGeometry(
							FVector2D::ZeroVector,
							FVector2D(AllottedGeometry.GetLocalSize().X, AllottedGeometry.GetLocalSize().Y)),
						CurrentFillImage,
						DrawEffects,
						FillColorAndOpacitySRGB
					);

					OutDrawElements.PopClip();
				}
				break;
			}
		case EFillMode::FillFromCenter:
			{
				const float HalfFrac = ClampedFraction / 2.0f;
				if (PushTransformedClipExt(OutDrawElements, AllottedGeometry, BorderPaddingRef, FVector2D(0.5, 0.5),
				                        FSlateRect(HalfFrac, HalfFrac, HalfFrac, HalfFrac)))
				{
					// Draw Fill
					FSlateDrawElement::MakeBox(
						OutDrawElements,
						RetLayerId++,
						AllottedGeometry.ToPaintGeometry(
							FVector2D(
								(AllottedGeometry.GetLocalSize().X * 0.5f) - ((AllottedGeometry.GetLocalSize().X * (
									ClampedFraction)) * 0.5), 0.0f),
							FVector2D(AllottedGeometry.GetLocalSize().X * (ClampedFraction),
							          AllottedGeometry.GetLocalSize().Y)),
						CurrentFillImage,
						DrawEffects,
						FillColorAndOpacitySRGB
					);

					OutDrawElements.PopClip();
				}
				break;
			}
		case EFillMode::TopToBottom:
			{
				if (PushTransformedClipExt(OutDrawElements, AllottedGeometry, BorderPaddingRef, FVector2D(0, 0),
				                        FSlateRect(0, 0, 1, ClampedFraction)))
				{
					// Draw Fill
					FSlateDrawElement::MakeBox(
						OutDrawElements,
						RetLayerId++,
						AllottedGeometry.ToPaintGeometry(
							FVector2D::ZeroVector,
							FVector2D(AllottedGeometry.GetLocalSize().X, AllottedGeometry.GetLocalSize().Y)),
						CurrentFillImage,
						DrawEffects,
						FillColorAndOpacitySRGB
					);

					OutDrawElements.PopClip();
				}
				break;
			}
		case EFillMode::BottomToTop:
			{
				if (PushTransformedClipExt(OutDrawElements, AllottedGeometry, BorderPaddingRef, FVector2D(0, 1),
				                        FSlateRect(0, ClampedFraction, 1, 0)))
				{
					FSlateRect ClippedAllotedGeometry = FSlateRect(FVector2D(AllottedGeometry.AbsolutePosition),
																FVector2D(AllottedGeometry.AbsolutePosition) + AllottedGeometry.
					                                               GetLocalSize() * AllottedGeometry.Scale);
					ClippedAllotedGeometry.Top = ClippedAllotedGeometry.Bottom - ClippedAllotedGeometry.GetSize().Y *
						ClampedFraction;

					// Draw Fill
					FSlateDrawElement::MakeBox(
						OutDrawElements,
						RetLayerId++,
						AllottedGeometry.ToPaintGeometry(
							FVector2D::ZeroVector,
							FVector2D(AllottedGeometry.GetLocalSize().X, AllottedGeometry.GetLocalSize().Y)),
						CurrentFillImage,
						DrawEffects,
						FillColorAndOpacitySRGB
					);

					OutDrawElements.PopClip();
				}
				break;
			}
		case EFillMode::ClockWise:
		case EFillMode::CounterClockWise:
			{
				TOptional<float> StartAngleFraction = StartAngle.Get();
				if (!StartAngleFraction.IsSet())
				{
					break;
				}
				if (PushTransformedClipExt(OutDrawElements, AllottedGeometry, BorderPaddingRef, FVector2D(0, 0),
				                        FSlateRect(0, 0, 1, 1)))
				{
					// Draw Fill
					const FVector2D Pos = AllottedGeometry.GetAbsolutePosition();
					const FVector2D Size = AllottedGeometry.GetAbsoluteSize();
					const FVector2D LocalSize = AllottedGeometry.GetLocalSize();

					const float BoxSize = FMath::Min(Size.X, Size.Y);
					FVector2D LocalOffset = FVector2D(FMath::Max(0.f, 0.5f * (Size.X - Size.Y)),
					                                  FMath::Max(0.f, 0.5f * (Size.Y - Size.X)));
					// if (ProgressFraction.GetValue() == 1)
					// {
					// 	FSlateDrawElement::MakeBox(
					// 		OutDrawElements,
					// 		RetLayerId++,
					// 		AllottedGeometry.ToPaintGeometry(
					// 			LocalOffset,
					// 			LocalSize),
					// 		CurrentFillImage,
					// 		DrawEffects,
					// 		FillColorAndOpacitySRGB
					// 	);
					// }
					// else
					if (ProgressFraction.GetValue() != 0)
					{
						TArray<float> Pts;
						const TArray<float> Corners = {
							-1.875f, -1.625f, -1.375f, -1.125f, -0.875f, -0.625f, -0.375f, -0.125f,
							0.125f, 0.375f, 0.625f, 0.875f, 1.125f, 1.375f, 1.625f, 1.875f
						};
						const float Direction = ComputedBarFillType == EFillMode::CounterClockWise ? -1 : 1;
						float Start = StartAngleFraction.GetValue();
						const float End = Start + Direction * ProgressFraction.GetValue();

						Pts.Add(Start);
						Pts.Add(End);

						const float From = FMath::Min(Start, End);
						const float To = FMath::Max(Start, End);
						for (int i = 0; i < Corners.Num(); i++)
						{
							if (Corners[i] > From && Corners[i] < To)
							{
								Pts.Add(Corners[i]);
							}
						}
						Pts.Sort();
						// Algo::Reverse(Pts);
						const float Radius = BoxSize * 0.5f;
						TArray<FSlateVertex> Vertices;
						Vertices.Reserve(Pts.Num() + 1);


						const FColor FinalColorAndOpacity = FillColorAndOpacitySRGB.ToFColor(true);

						const FSlateResourceHandle Handle = FSlateApplication::Get().GetRenderer()->
							GetResourceHandle(*CurrentFillImage);
						const FSlateShaderResourceProxy* ResourceProxy = Handle.GetResourceProxy();
						
						FVector2D Tiling = FVector2D::UnitVector;
						FVector2D StartUV = FVector2D::ZeroVector;
						FVector2D SizeUV = FVector2D::UnitVector;

						switch (CurrentFillImage->DrawAs)
						{
						case ESlateBrushDrawType::Box:
							// SizeUV = FVector2D::ZeroVector;
							break;
						case ESlateBrushDrawType::Image:
						case ESlateBrushDrawType::Border:
							if (ResourceProxy)
							{
								StartUV = FVector2D(ResourceProxy->StartUV);
								SizeUV = FVector2D(ResourceProxy->SizeUV);
								// ToDO Try To Get BrushTiling
								// const ESlateBrushTileType::Type TilingRule = DrawElementPayload.GetBrushTiling();
								Tiling = FVector2D::UnitVector;
							}
							break;
						default:
							break;
						}

						FVector2D Center = Pos + 0.5f * Size;
						// Add center vertex
						Vertices.AddZeroed();
						FSlateVertex& CenterVertex = Vertices.Last();

						CenterVertex.Position = Center;
						FVector2D CenterUV = StartUV + 0.5f * SizeUV;
						CenterVertex.TexCoords[0] = CenterUV.X;
						CenterVertex.TexCoords[1] = CenterUV.Y;
						CenterVertex.TexCoords[2] = Tiling.X;
						CenterVertex.TexCoords[3] = Tiling.Y;
						// CenterVertex.MaterialTexCoords = FVector2D(0.5f, 0.5f);
						CenterVertex.Color = FinalColorAndOpacity;

						TArray<FVector2D> UVs;
						for (int i = 0; i < Pts.Num(); i++)
						{
							const FVector2D UV = UnitValToUV(Pts[i]);
							Vertices.AddZeroed();
							FSlateVertex& OuterVert = Vertices.Last();
							OuterVert.Position = Center + (UV - FVector2D(0.5f)) * Radius * 2;
							FVector2D UVPrim = UV * SizeUV + StartUV;
							OuterVert.TexCoords[0] = UVPrim.X;
							OuterVert.TexCoords[1] = UVPrim.Y;
							OuterVert.TexCoords[2] = Tiling.X;
							OuterVert.TexCoords[3] = Tiling.Y;
							// OuterVert.MaterialTexCoords = UV;
							OuterVert.Color = FinalColorAndOpacity;
						}
						TArray<SlateIndex> Indices;
						for (int i = 0; i <= Pts.Num() - 1; ++i)
						{
							Indices.Add(0);
							Indices.Add(i);
							Indices.Add(i + 1);
						}

						FSlateDrawElement::MakeCustomVerts(
							OutDrawElements,
							RetLayerId++,
							Handle,
							Vertices,
							Indices,
							nullptr,
							0,
							0
						);
					}

					OutDrawElements.PopClip();
				}
				break;;
			}

		case EFillMode::LeftToRight:
		default:
			{
				if (PushTransformedClipExt(OutDrawElements, AllottedGeometry, BorderPaddingRef, FVector2D(0, 0),
				                        FSlateRect(0, 0, ClampedFraction, 1)))
				{
					// Draw Fill
					FSlateDrawElement::MakeBox(
						OutDrawElements,
						RetLayerId++,
						AllottedGeometry.ToPaintGeometry(
							FVector2D::ZeroVector,
							FVector2D(AllottedGeometry.GetLocalSize().X, AllottedGeometry.GetLocalSize().Y)),
						CurrentFillImage,
						DrawEffects,
						FillColorAndOpacitySRGB
					);

					OutDrawElements.PopClip();
				}
				break;
			}
		}
	}
	else
	{
		const FSlateBrush* CurrentMarqueeImage = GetMarqueeImage();

		// Draw Marquee
		const float MarqueeAnimOffset = CurrentMarqueeImage->ImageSize.X * MarqueeOffset;
		const float MarqueeImageSize = CurrentMarqueeImage->ImageSize.X;

		if (PushTransformedClipExt(OutDrawElements, AllottedGeometry, BorderPaddingRef, FVector2D(0, 0),
		                        FSlateRect(0, 0, 1, 1)))
		{
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				RetLayerId++,
				AllottedGeometry.ToPaintGeometry(
					FVector2D(MarqueeAnimOffset - MarqueeImageSize, 0.0f),
					FVector2D(AllottedGeometry.GetLocalSize().X + MarqueeImageSize, AllottedGeometry.GetLocalSize().Y)),
				CurrentMarqueeImage,
				DrawEffects,
				CurrentMarqueeImage->TintColor.GetSpecifiedColor() * ColorAndOpacitySRGB
			);

			OutDrawElements.PopClip();
		}
	}

	return RetLayerId - 1;
}

FVector2D SProgressBarExt::ComputeDesiredSize(float) const
{
	return GetMarqueeImage()->ImageSize;
}

bool SProgressBarExt::ComputeVolatility() const
{
	return SLeafWidget::ComputeVolatility() ||
		Percent.IsBound() || StartAngle.IsBound() ||
		FillColorAndOpacity.IsBound() || BorderPadding.IsBound();
}

void SProgressBarExt::SetPercent(TAttribute<TOptional<float>> InPercent)
{
	if (!Percent.IdenticalTo(InPercent))
	{
		Percent = InPercent;
		UpdateMarqueeActiveTimer();
		Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}
}

void SProgressBarExt::SetStartAngle(TAttribute<TOptional<float>> InStartAngle)
{
	if (!StartAngle.IdenticalTo(InStartAngle))
	{
		StartAngle = InStartAngle;
		UpdateMarqueeActiveTimer();
		Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}
}

void SProgressBarExt::SetStyle(const FProgressBarStyle* InStyle)
{
	Style = InStyle;

	if (Style == nullptr)
	{
		FArguments Defaults;
		Style = Defaults._Style;
	}

	check(Style);

	Invalidate(EInvalidateWidget::Layout);
}

void SProgressBarExt::SetBarFillType(EFillMode::Type InBarFillType)
{
	if (BarFillType != InBarFillType)
	{
		BarFillType = InBarFillType;
		Invalidate(EInvalidateWidget::Paint);
	}
}

void SProgressBarExt::SetFillColorAndOpacity(TAttribute<FSlateColor> InFillColorAndOpacity)
{
	if (!FillColorAndOpacity.IdenticalTo(InFillColorAndOpacity))
	{
		FillColorAndOpacity = InFillColorAndOpacity;
		Invalidate(EInvalidateWidget::Paint);
	}
}

void SProgressBarExt::SetBorderPadding(TAttribute<FVector2D> InBorderPadding)
{
	if (!BorderPadding.IdenticalTo(InBorderPadding))
	{
		BorderPadding = InBorderPadding;
		Invalidate(EInvalidateWidget::Layout);
	}
}

void SProgressBarExt::SetBackgroundImage(const FSlateBrush* InBackgroundImage)
{
	if (BackgroundImage != InBackgroundImage)
	{
		BackgroundImage = InBackgroundImage;
		Invalidate(EInvalidateWidget::Layout);
	}
}

void SProgressBarExt::SetFillImage(const FSlateBrush* InFillImage)
{
	if (FillImage != InFillImage)
	{
		FillImage = InFillImage;
		Invalidate(EInvalidateWidget::Layout);
	}
}

void SProgressBarExt::SetMarqueeImage(const FSlateBrush* InMarqueeImage)
{
	if (MarqueeImage != InMarqueeImage)
	{
		MarqueeImage = InMarqueeImage;
		Invalidate(EInvalidateWidget::Layout);
	}
}

void SProgressBarExt::SetActiveTimerTickRate(float TickRate)
{
	if (CurrentTickRate != TickRate || !ActiveTimerHandle.IsValid())
	{
		CurrentTickRate = TickRate;

		TSharedPtr<FActiveTimerHandle> SharedActiveTimerHandle = ActiveTimerHandle.Pin();
		if (SharedActiveTimerHandle.IsValid())
		{
			UnRegisterActiveTimer(SharedActiveTimerHandle.ToSharedRef());
		}

		UpdateMarqueeActiveTimer();
	}
}

void SProgressBarExt::UpdateMarqueeActiveTimer()
{
	if (ActiveTimerHandle.IsValid())
	{
		UnRegisterActiveTimer(ActiveTimerHandle.Pin().ToSharedRef());
	}

	if (!Percent.IsBound() && !Percent.Get().IsSet())
	{
		// If percent is not bound or set then its marquee. Set the timer
		ActiveTimerHandle = RegisterActiveTimer(CurrentTickRate,
		                                        FWidgetActiveTimerDelegate::CreateSP(
			                                        this, &SProgressBarExt::ActiveTick));
	}
}

EActiveTimerReturnType SProgressBarExt::ActiveTick(double InCurrentTime, float InDeltaTime)
{
	MarqueeOffset = InCurrentTime - FMath::FloorToDouble(InCurrentTime);

	const TOptional<float> PercentFraction = Percent.Get();
	if (PercentFraction.IsSet())
	{
		SetActiveTimerTickRate(MinimumTickRate);
	}
	else
	{
		SetActiveTimerTickRate(0.0f);
	}

	Invalidate(EInvalidateWidget::Paint);

	return EActiveTimerReturnType::Continue;
}

const FSlateBrush* SProgressBarExt::GetBackgroundImage() const
{
	return BackgroundImage ? BackgroundImage : &Style->BackgroundImage;
}

const FSlateBrush* SProgressBarExt::GetFillImage() const
{
	return FillImage ? FillImage : &Style->FillImage;
}

const FSlateBrush* SProgressBarExt::GetMarqueeImage() const
{
	return MarqueeImage ? MarqueeImage : &Style->MarqueeImage;
}
