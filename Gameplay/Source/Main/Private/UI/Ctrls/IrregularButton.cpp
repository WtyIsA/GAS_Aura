// Fill out your copyright notice in the Description page of Project Settings.


#include "IrregularButton.h"
#include "SIrregularButton.h"
#include "Components/ButtonSlot.h"

UIrregularButton::UIrregularButton(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),AdvancedHitTexture(NULL), AdvancedHitAlpha(1)
{
		
}

void UIrregularButton::SetAdvancedHitTexture(UTexture2D* InTexture) 
{ 
	AdvancedHitTexture = InTexture;
	if (MyButton.IsValid()) (static_cast<SIrregularButton*>(MyButton.Get()))->SetAdvancedHitTexture(AdvancedHitTexture);
}
void UIrregularButton::SetAdvancedHitAlpha(int InAlpha)
{
	AdvancedHitAlpha = InAlpha;
	if (MyButton.IsValid()) (static_cast<SIrregularButton*>(MyButton.Get()))->SetAdvancedHitAlpha(AdvancedHitAlpha);
}

void UIrregularButton::SynchronizeProperties() {
	Super::SynchronizeProperties();
	(static_cast<SIrregularButton*>(MyButton.Get()))->SetAdvancedHitTexture(AdvancedHitTexture);
	(static_cast<SIrregularButton*>(MyButton.Get()))->SetAdvancedHitAlpha(AdvancedHitAlpha);
}
TSharedRef<SWidget> UIrregularButton::RebuildWidget() {
	TSharedPtr<SIrregularButton> OvodusButton = SNew(SIrregularButton)
        .OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
        .OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
        .OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
        .OnHovered_UObject(this, &ThisClass::SlateHandleHovered)
        .OnUnhovered_UObject(this, &ThisClass::SlateHandleUnhovered)
        .ButtonStyle(&WidgetStyle)
        .ClickMethod(ClickMethod)
        .TouchMethod(TouchMethod)
        .IsFocusable(IsFocusable)
        ;

	OvodusButton->SetAdvancedHitTexture(AdvancedHitTexture);
	OvodusButton->SetAdvancedHitAlpha(AdvancedHitAlpha);

	MyButton = OvodusButton;

	if (GetChildrenCount() > 0) Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());

	return MyButton.ToSharedRef();
}