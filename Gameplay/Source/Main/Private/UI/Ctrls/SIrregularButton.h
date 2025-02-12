#pragma once
#include <Widgets/Input/SButton.h>

class  SIrregularButton : public SButton
{
public:
    SIrregularButton();

    UTexture2D* AdvancedHitTexture;
    int AdvancedHitAlpha;

    void SetAdvancedHitTexture(UTexture2D* InTexture) 
    { 
        AdvancedHitTexture = InTexture; 
    }
    void SetAdvancedHitAlpha(int InAlpha) 
    { 
        AdvancedHitAlpha = InAlpha; 
    }

    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override 
    {
        if (!IsHovered()) return FReply::Unhandled();
        return SButton::OnMouseButtonDown(MyGeometry, MouseEvent);
    }
    virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override {
        if (!IsHovered()) return FReply::Unhandled();
        return SButton::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
    }
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override {
        if (!IsHovered()) return FReply::Unhandled();
        return SButton::OnMouseButtonUp(MyGeometry, MouseEvent);
    }
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override {
        if (AdvancedHitTexture) return;
        return SButton::OnMouseEnter(MyGeometry, MouseEvent);
    }
    virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override {
        return SButton::OnMouseLeave(MouseEvent);
    }

    virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
    virtual TSharedPtr<IToolTip> GetToolTip() override { return (IsHovered() ? SWidget::GetToolTip() : NULL); }
};
