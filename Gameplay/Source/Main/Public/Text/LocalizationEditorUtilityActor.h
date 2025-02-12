
#pragma once

#include "CoreMinimal.h"

#if WITH_EDITOR
#include "Blueprint/UserWidget.h"
#include "WidgetBlueprint.h"
#include "Engine/ObjectLibrary.h"
#endif

#include "LocalizationEditorUtilityActor.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogLocalizationEditorUtilityActor, Log, All);

UCLASS()
class ALocalizationEditorUtilityActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Tools_Localization")
	FDirectoryPath DirectoryPath;

	UFUNCTION(CallInEditor, Category = "Tools_Localization")
	void SetNoneTranslateChineseToEmpty();

#if WITH_EDITOR
	static bool ContainChinese(const FString& InSource);
	void CollectText(TMap<FString,FString>& InCollectMap, UWidget* InTargetWidget, const FAssetData& InAssetData, UWidgetBlueprint* InPanel,bool& bOutChanged,FText InTargetText,TFunctionRef<void(FText)> SetText);
	
	template <typename T>
	static void GetAssetDataByType(T DataType, FDirectoryPath DirectoryPath, TArray<FAssetData>& ArrayAssetData)
	{
		const FString BinariesDir = FString(FPaths::ProjectContentDir());
		FString RelativePath = DirectoryPath.Path;
		FPaths::MakePathRelativeTo(RelativePath, *BinariesDir);
		const FString AssetPath = FString::Format(TEXT("/Game/{0}"), {RelativePath});

		if (UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(DataType, true, GIsEditor))
		{
			ObjectLibrary->AddToRoot();
			ObjectLibrary->LoadAssetDataFromPath(AssetPath);
			ObjectLibrary->GetAssetDataList(ArrayAssetData);
		}
		else
		{
			UE_LOG(LogLocalizationEditorUtilityActor,Error,TEXT("CreateLibrary failed."))
		}
	}
#endif
};
