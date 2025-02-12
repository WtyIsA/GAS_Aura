// Fill out your copyright notice in the Description page of Project Settings.


#include "Text/LocalizationEditorUtilityActor.h"

#if WITH_EDITOR

#include "Blueprint/WidgetTree.h"
#include "Components/EditableText.h"
#include "Components/EditableTextBox.h"
#include "Components/MultiLineEditableText.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Misc/FileHelper.h"
#endif

#if WITH_EDITOR
void ALocalizationEditorUtilityActor::CollectText(TMap<FString,FString>& InCollectMap, UWidget* InTargetWidget, const FAssetData& InAssetData, UWidgetBlueprint* InPanel,bool& bOutChanged,FText InTargetText,TFunctionRef<void(FText)> SetText)
{
	UFunction* GetTextFunc = InTargetWidget->GetClass()->FindFunctionByName("GetText");
	if (!GetTextFunc)
	{
		UE_LOG(LogLocalizationEditorUtilityActor,Error,TEXT("No GetText Function : %s."),*(InAssetData.PackagePath.ToString() / InPanel->GetName() + ":" + InTargetWidget->GetName()))
		return;
	}

	if (InTargetText.IsCultureInvariant() && ContainChinese(InTargetText.ToString()))
	{
		InCollectMap.Emplace(InAssetData.PackagePath.ToString() / InPanel->GetName() + ":" + InTargetWidget->GetName(),InTargetText.ToString());
		static const FText EmptyText = FText::AsCultureInvariant("");
		SetText(EmptyText);
		bOutChanged = true;
	}
}
#endif

void ALocalizationEditorUtilityActor::SetNoneTranslateChineseToEmpty()
{
#if WITH_EDITOR
	if (DirectoryPath.Path.IsEmpty())
	{
		UE_LOG(LogLocalizationEditorUtilityActor,Error,TEXT("Path is empty."))
		return;
	}

	TArray<FAssetData> arrayAssetData;
	GetAssetDataByType(UWidgetBlueprint::StaticClass(), DirectoryPath, arrayAssetData);

	TMap<FString,FString> PathOldContent;
	
	UWidgetBlueprint* CurUserWidget;
	for (int index = 0; index < arrayAssetData.Num(); ++index)
	{
		FAssetData ad = arrayAssetData[index];
		CurUserWidget = Cast<UWidgetBlueprint>(ad.GetAsset());
		if (CurUserWidget)
		{
			bool bHasTextChanged = false;
			CurUserWidget->WidgetTree->ForEachWidgetAndDescendants([&](UWidget* InWidget)
			{
				// Only consider UTextBlock URichTextBlock UEditableText UMultiLineEditableText UEditableTextBox UMultiLineEditableTextBox
				if (UTextBlock* tb = Cast<UTextBlock>(InWidget))
				{
					CollectText(PathOldContent,tb,ad,CurUserWidget,bHasTextChanged,tb->GetText(),[tb](FText InEmptyText){tb->SetText(InEmptyText);});
				}
				else if(URichTextBlock* rtb = Cast<URichTextBlock>(InWidget))
				{
					CollectText(PathOldContent,rtb,ad,CurUserWidget,bHasTextChanged,rtb->GetText(),[rtb](FText InEmptyText){rtb->SetText(InEmptyText);});
				}
				else if(UEditableText* et = Cast<UEditableText>(InWidget))
				{
					CollectText(PathOldContent,et,ad,CurUserWidget,bHasTextChanged,et->GetText(),[et](FText InEmptyText){et->SetText(InEmptyText);});
				}
				else if(UMultiLineEditableText* met = Cast<UMultiLineEditableText>(InWidget))
				{
					CollectText(PathOldContent,met,ad,CurUserWidget,bHasTextChanged,met->GetText(),[met](FText InEmptyText){met->SetText(InEmptyText);});
				}
				else if(UEditableTextBox* etb = Cast<UEditableTextBox>(InWidget))
				{
					CollectText(PathOldContent,etb,ad,CurUserWidget,bHasTextChanged,etb->GetText(),[etb](FText InEmptyText){etb->SetText(InEmptyText);});
				}
				else if(UMultiLineEditableTextBox* metb = Cast<UMultiLineEditableTextBox>(InWidget))
				{
					CollectText(PathOldContent,metb,ad,CurUserWidget,bHasTextChanged,metb->GetText(),[metb](FText InEmptyText){metb->SetText(InEmptyText);});
				}
			});

			if (bHasTextChanged)
			{
				CurUserWidget->Modify();
				CurUserWidget->PostEditChange();
				CurUserWidget->MarkPackageDirty();
			}
		}
	}

	if (PathOldContent.Num() > 0)
	{
		FString FileContent;
		for (auto pair : PathOldContent)
		{
			FileContent = FileContent + pair.Key + ",\"" + pair.Value + "\"\r\n";
		}
		FString FilePath = DirectoryPath.Path / "EmptyTextRecord.csv";
		if (!FFileHelper::SaveStringToFile(FileContent,*FilePath,FFileHelper::EEncodingOptions::ForceUTF8))
		{
			FString TempFilePath = DirectoryPath.Path / "EmptyTextRecord" + FDateTime::Now().ToString() + ".csv";
			UE_LOG(LogLocalizationEditorUtilityActor,Error,TEXT("Save Out Put failed at path : %s, try to save at path : %s."),*FilePath,*TempFilePath)
			FFileHelper::SaveStringToFile(FileContent,*TempFilePath,FFileHelper::EEncodingOptions::ForceUTF8);
		}
		else
		{
			UE_LOG(LogLocalizationEditorUtilityActor,Display,TEXT("Finished see file : %s."),*FilePath)
		}
	}
	else
	{
		UE_LOG(LogLocalizationEditorUtilityActor,Display,TEXT("No text find."))
	}
#endif
}

#if WITH_EDITOR
bool ALocalizationEditorUtilityActor::ContainChinese(const FString& InSource)
{
	FString content;
	//U+4E00..U+9FA5
	for (int k = 0; k < InSource.Len(); k++)
	{
		int c = InSource[k];
		if (c >= 19968 && c <= 40869)
			return true;
	}
    
	return false;
}
#endif