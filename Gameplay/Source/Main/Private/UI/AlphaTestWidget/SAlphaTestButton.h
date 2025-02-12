//gjy
#pragma once

#include "CoreMinimal.h"

class SAlphaTestButton : public SButton
{
	
public:
	SAlphaTestButton()
		:AdvancedHitTexture(nullptr)
		,TestAlpha(0)
	{
		SetTouchMethod(EButtonTouchMethod::PreciseTap);
	}

	void SetAdvancedHitTexture(UTexture2D* InAdvancedHitTexture) { AdvancedHitTexture = InAdvancedHitTexture; }
	void SetTestAlpha(uint8 InTestAlpha) { TestAlpha = InTestAlpha; }
	const bool AnyParentHasCapture() const;
	
	virtual bool CursorPosAlphaNotZero(const FVector2D& CursorPositionInScreenSpace) const;

	virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	void CallButtonRelease();
private:
	UTexture2D* AdvancedHitTexture;
	uint8 TestAlpha;
	TOptional<bool> bHasAlphaTestParent;
	uint8 bEnterLeave : 1;

	bool HasAlphaTestParent();
	bool ShouldHandle(const FVector2D& CursorPositionInScreenSpace);
};
