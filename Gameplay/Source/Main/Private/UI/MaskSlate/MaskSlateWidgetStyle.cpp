
#include "MaskSlateWidgetStyle.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"

FMaskSlateStyle::FMaskSlateStyle()
: BackgroundImage()
, MaskMatBrush()
{
	if (!GIsRetrievingVTablePtr)
	{
		UMaterial* Mat = LoadObject<UMaterial>(nullptr, TEXT("/Game/UI/UI_Common/MaskSlate/MaskMaterial.MaskMaterial"), nullptr, LOAD_None, nullptr);
		if (Mat)
		{
			UMaterialInstanceDynamic* DyMat = nullptr;
			DyMat = UMaterialInstanceDynamic::Create(Mat, GetTransientPackage());
			if (DyMat)
			{
				MaskMatBrush.SetResourceObject(DyMat);
			}
			else
			{
				MaskMatBrush.SetResourceObject(Mat);
			}
		}
	}
}

FMaskSlateStyle::~FMaskSlateStyle()
{
}

const FName FMaskSlateStyle::TypeName(TEXT("FMaskSlateStyle"));

const FMaskSlateStyle& FMaskSlateStyle::GetDefault()
{
	static FMaskSlateStyle Default;
	return Default;
}

void FMaskSlateStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	OutBrushes.Add(&BackgroundImage);
	OutBrushes.Add(&MaskMatBrush);
}

void FMaskSlateStyle::ReIndexClip()
{
	for (uint8 i = 0; i < MaskClips.Num(); i++)
	{
		MaskClips[i].SetIndex(i);
	}
}

const UTexture2D* FMaskSlateStyle::GetMaskTextureByIdx(const int32& Index) const
{
	if (MaskClips.Num() > Index)
	{
		return MaskClips[Index].GetMaskTexture();
	}
	return nullptr;
}

bool FMaskSlateStyle::SetMaskTextureByIdx(const int32& Index, UTexture2D* Texture)
{
	if (MaskClips.Num() > Index)
	{
		MaskClips[Index].SetMaskTexture(Texture);
		return true;
	}
	return false;
}

bool FMaskSlateStyle::SetMaskSize(const int32& Index, const FVector2D& Size)
{
	if (MaskClips.Num() > Index)
	{
		MaskClips[Index].SetSize(Size);
		return true;
	}
	return false;
}

bool FMaskSlateStyle::SetMaskSizeXY(const int32& Index, const float& X, const float& Y)
{
	if (MaskClips.Num() > Index)
	{
		return true;
	}
	return false;
}

bool FMaskSlateStyle::SetMaskPos(const int32& Index, const FVector2D& Pos)
{
	if (MaskClips.Num() > Index)
	{
		MaskClips[Index].SetPosition(Pos);
		return true;
	}
	return false;
}

bool FMaskSlateStyle::SetMaskPosXY(const int32& Index, const float& X, const float& Y)
{
	if (MaskClips.Num() > Index)
	{
		return true;
	}
	return false;
}

bool FMaskSlateStyle::SetMaskPosSize(const int32& Index, const FVector4& PosSize)
{
	if (MaskClips.Num() > Index)
	{
		return true;
	}
	return false;
}

bool FMaskSlateStyle::SetMaskPosSizeXYZW(const int32& Index, const float& X, const float& Y, const float& Z, const float& W)
{
	if (MaskClips.Num() > Index)
	{
		return true;
	}
	return false;
}

bool FMaskSlateStyle::EnableMaskClickClip(const int32& Index, bool Enable)
{
	if (MaskClips.Num() > Index)
	{
		MaskClips[Index].SetEnable(Enable);
		return true;
	}
	return false;
}

int32 FMaskSlateStyle::AddMaskClickClip(const FVector2D& Position, const FVector2D& Size, UTexture2D* Mask)
{
	int32 Count = MaskClips.Num();
	if (Count < MAX_MASK_CLIP_COUNT)
	{
		MaskClips.Add(FMaskClip(Count, Position, Size, Mask));
		return Count;
	}
	return -1;
}

bool FMaskSlateStyle::RemoveMaskClickClip(const int32& ClipIndex)
{
	if (ClipIndex >= 0 && ClipIndex < MaskClips.Num())
	{
		MaskClips.RemoveAt(ClipIndex);
		return true;
	}
	else
	{
		UE_LOG(LogInit, Error, TEXT("ERROR: FMaskWidgetStyle::RemoveMaskClickClip invalid ClipIndex = %d, MaskClips.Num() = %d"), ClipIndex, MaskClips.Num());
	}
	return false;
}