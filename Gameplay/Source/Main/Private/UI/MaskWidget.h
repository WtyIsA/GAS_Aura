
#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "MaskSlate/MaskSlateWidgetStyle.h"
#include "SSMaskWidget.h"
#include "Widgets/SWidget.h"
#include "MaskWidget.generated.h"

class USlateBrushAsset;

/**
 * 
 */
UCLASS()
class MAIN_API UMaskWidget : public UWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style", meta = (DisplayName = "Style"))
		FMaskSlateStyle WidgetStyle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (sRGB = "true"))
		FLinearColor BgColorAndOpacity;

	UPROPERTY()
		FGetLinearColor BgColorAndOpacityDelegate;

	UPROPERTY(BlueprintReadOnly)
		FSlateBrush Brush;

public:

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetBgColorAndOpacity(FLinearColor InColorAndOpacity);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetBgOpacity(float InOpacity);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetBgImage(UTexture2D* Tex, bool bMatchSize = false);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetBrushTintColor(FSlateColor TintColor);

	UFUNCTION(BlueprintCallable, Category = "MaskClip")
		void SetMaskImage(const int32& ClipIndex, UTexture2D* Tex);

	UFUNCTION(BlueprintCallable, Category = "MaskClip")
		void SetMaskPos(const int32& ClipIndex, const FVector2D& Pos);

	UFUNCTION(BlueprintCallable, Category = "MaskClip")
		void SetMaskPosXY(const int32& ClipIndex, const float& X, const float& Y);

	UFUNCTION(BlueprintCallable, Category = "MaskClip")
		void SetMaskSize(const int32& ClipIndex, const FVector2D& Size);

	UFUNCTION(BlueprintCallable, Category = "MaskClip")
		void SetMaskSizeXY(const int32& ClipIndex, const float& X, const float& Y);

	UFUNCTION(BlueprintCallable, Category = "MaskClip")
		void SetMaskPosSize(const int32& ClipIndex, const FVector4& PosSize);

	UFUNCTION(BlueprintCallable, Category = "MaskClip")
		void SetMaskPosSizeXYZW(const int32& ClipIndex, const float& X, const float& Y, const float& Z, const float& W);

	UFUNCTION(BlueprintCallable, Category = "MaskClip")
		void EnableMaskClickClip(const int32& ClipIndex, bool Enable);

	UFUNCTION(BlueprintCallable, Category = "MaskClip")
		int32 AddMaskClickClip(const FVector2D& Position, const FVector2D& Size, UTexture2D* Mask = nullptr);

	UFUNCTION(BlueprintCallable, Category = "MaskClip")
		bool RemoveMaskClickClip(const int32& ClipIndex);

	UFUNCTION(BlueprintCallable, Category = "MaskClip")
		UMaterialInterface* GetDynamicMaterial();

public:

	UMaskWidget(const FObjectInitializer& ObjectInitializer);

	virtual void SynchronizeProperties() override;

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:

	TSharedPtr<SSMaskWidget> MyMask;

	virtual TSharedRef<SWidget> RebuildWidget() override;

	PROPERTY_BINDING_IMPLEMENTATION(FSlateColor, BgColorAndOpacity);
};
