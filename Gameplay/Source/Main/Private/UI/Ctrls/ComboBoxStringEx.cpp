#include "ComboBoxStringEx.h"


UComboBoxStringEx::UComboBoxStringEx(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Placement = MenuPlacement_ComboBox;
}

TSharedRef<SWidget> UComboBoxStringEx::RebuildWidget()
{
#if UE_SERVER
	return Super::RebuildWidget();
#else
	Super::RebuildWidget();
	if (MyComboBox.IsValid())
	{
		if (hideScrollbar)
		{
			if (MyComboBox->ComboListView.IsValid())
			{
				MyComboBox->ComboListView->SetScrollbarVisibility(EVisibility::Collapsed);
			}
		}

		MyComboBox->SetMenuPlacement(Placement);
	}
	if (!DownArrowTransform.IsIdentity())
	UpdateDownArrowRenderTransform();
	
	return MyComboBox.ToSharedRef();
#endif
}


TSharedRef<SWidget> UComboBoxStringEx::HandleGenerateWidget(TSharedPtr<FString> Item) const
{
	FString StringItem = Item.IsValid() ? *Item : FString();

	// Call the user's delegate to see if they want to generate a custom widget bound to the data source.
	if ( !IsDesignTime() && OnGenerateWidgetEvent.IsBound() )
	{
		UWidget* Widget = OnGenerateWidgetEvent.Execute(StringItem);
		if ( Widget != NULL )
		{
			return Widget->TakeWidget();
		}
	}

	if (StringItem.StartsWith("cfg."))
	{
		FText TranslatedText = FInternationalization::ForUseOnlyByLocMacroAndGraphNodeTextLiterals_CreateText(*StringItem, TEXT("MultiLanguageTranslation"), *StringItem);
		return SNew(STextBlock)
			.Text(TranslatedText)
			.Font(Font)
			.Justification(Justification);
	}
	// If a row wasn't generated just create the default one, a simple text block of the item's name.
	return SNew(STextBlock)
        .Text(FText::FromString(StringItem))
        .Font(Font)
		.Justification(Justification);
}

void UComboBoxStringEx::SetPlacement(TEnumAsByte<EMenuPlacement> InPlacement)
{
	Placement = InPlacement;
	if (MyComboBox.IsValid())
	{
		return MyComboBox->SetMenuPlacement(Placement);
	}
}

void UComboBoxStringEx::SetDownArrowAngle(float InAngle)
{
	DownArrowTransform.Angle = InAngle;
	UpdateDownArrowRenderTransform();
}

void UComboBoxStringEx::SetDownArrowRenderTransform(FWidgetTransform InDownArrowTransform)
{
	DownArrowTransform = InDownArrowTransform;
	UpdateDownArrowRenderTransform();
}

void UComboBoxStringEx::UpdateDownArrowRenderTransform()
{
	if (MyComboBox->GetAllChildren()->Num() > 0)
	{
		TSharedRef<SWidget> child = MyComboBox->GetAllChildren()->GetChildAt(0);
		if (child->GetChildren()->Num() > 0)
		{
			child = child->GetChildren()->GetChildAt(0);
			if (child->GetChildren()->Num() > 1)
			{
				child = child->GetChildren()->GetChildAt(1);
				if (child->GetChildren()->Num() > 1)
				{
					child = child->GetChildren()->GetChildAt(1);
					child->SetRenderTransform(DownArrowTransform.ToSlateRenderTransform());
				}
			}
		}
	}
}