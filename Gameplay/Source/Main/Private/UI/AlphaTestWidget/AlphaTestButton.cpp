// Fill out your copyright notice in the Description page of Project Settings.


#include "AlphaTestButton.h"
#include "SAlphaTestButton.h"
#include "Components/ButtonSlot.h"

TSharedRef<SWidget> UAlphaTestButton::RebuildWidget()
{
	TSharedPtr<SAlphaTestButton> AlphaTestButton = SNew(SAlphaTestButton)
	.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
	.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
	.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
	.OnHovered_UObject( this, &ThisClass::SlateHandleHovered )
	.OnUnhovered_UObject( this, &ThisClass::SlateHandleUnhovered )
	.ButtonStyle(&WidgetStyle)
	.ClickMethod(ClickMethod)
	.TouchMethod(TouchMethod)
	.PressMethod(PressMethod)
	.IsFocusable(IsFocusable)
	;

	if ( GetChildrenCount() > 0 )
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
	}
	
	AlphaTestButton->SetAdvancedHitTexture(HitTestTexture);
	AlphaTestButton->SetTestAlpha(TestAlpha);
	
	MyButton = AlphaTestButton;
	
	return MyButton.ToSharedRef();
}
