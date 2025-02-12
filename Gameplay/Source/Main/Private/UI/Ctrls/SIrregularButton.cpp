#include "SIrregularButton.h"


SIrregularButton::SIrregularButton(): AdvancedHitTexture(NULL), AdvancedHitAlpha(0)
{
    
}

FReply SIrregularButton::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {
	if (!AdvancedHitTexture)
		return SButton::OnMouseMove(MyGeometry, MouseEvent);
	bool bCanClick = true;
	FVector2D LocalPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	LocalPosition.X = floor(LocalPosition.X);
	LocalPosition.Y = floor(LocalPosition.Y);
	LocalPosition /= MyGeometry.GetLocalSize();
	int ImageWidth = AdvancedHitTexture->GetSizeX();
	LocalPosition.X *= ImageWidth;
	LocalPosition.Y *= AdvancedHitTexture->GetSizeY();
	int BufferPosition = (floor(LocalPosition.Y) * ImageWidth) + LocalPosition.X;
	FColor* ImageData =AdvancedHitTexture->PlatformData? (FColor*)((AdvancedHitTexture->PlatformData->Mips[0]).BulkData.Lock(LOCK_READ_ONLY)):nullptr;
	if (!ImageData)
	{
		bCanClick = false;
	}
	else
	{
		int a = ImageData[BufferPosition].A;
		if (a <= AdvancedHitAlpha)
			bCanClick = false;
	}
	if(AdvancedHitTexture->PlatformData)
		AdvancedHitTexture->PlatformData->Mips[0].BulkData.Unlock();
	if (bCanClick != IsHovered()) {
		if (bCanClick)
			SButton::OnMouseEnter(MyGeometry, MouseEvent);
		else
			SButton::OnMouseLeave(MouseEvent);
	}
	return SButton::OnMouseMove(MyGeometry, MouseEvent);
}
FCursorReply SIrregularButton::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const {
	if (!IsHovered())
		return FCursorReply::Unhandled();
	TOptional<EMouseCursor::Type> TheCursor = GetCursor();
	return (TheCursor.IsSet()) ? FCursorReply::Cursor(TheCursor.GetValue()) : FCursorReply::Unhandled();
}