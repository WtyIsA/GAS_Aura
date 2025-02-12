// Copyright 2018 Sam Bonifacio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "Engine/DataTable.h"
#include "URichTextLinkBlockDecorator.generated.h"

/**
 * 
 */

//USTRUCT()
//struct FRichHlinkRow : public FTableRowBase
//{
//	GENERATED_USTRUCT_BODY()
//
//public:
//	UPROPERTY(EditAnywhere, Category = Appearance)
//	FHyperlinkStyle HlinkStyle;
//};
//
//UCLASS()
//class UURichTextLinkBlockDecorator : public URichTextBlockDecorator
//{
//	GENERATED_BODY()
//public:
//	UURichTextLinkBlockDecorator(const FObjectInitializer& ObjectInitializer);
//
//	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;
//
//	virtual const FHyperlinkStyle* FindHyperLinkBrush(FName TagOrId, bool bWarnIfMissing);
//
//	UFUNCTION(BlueprintImplementableEvent)
//		void ClickFunc();
//
//	UPROPERTY(EditAnywhere)
//		FString HlName;
//
//	void SetHyperLinkContent(const FString* str);
//protected:
//
//	FRichHlinkRow* FindHlinkRow(FName TagOrId, bool bWarningIfMissing);
//
//	UPROPERTY(EditAnywhere, Category = Appearance, meta = (RowType = "RichHlinkRow"))
//	class UDataTable* HlinkSet;
//
//	UPROPERTY(BlueprintReadOnly)
//	FString HlContent;
//};

class URichTextLinkBlockDecorator;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRichTextEvent, FString, obj, FString, resPath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRichTextMapEvent, URichTextLinkBlockDecorator*, obj, int, buttonIndex);
DECLARE_DELEGATE_OneParam( FOnRichTextClickEvent, int );

UCLASS()
class URichTextLinkBlockDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()
public:
	URichTextLinkBlockDecorator(const FObjectInitializer& ObjectInitializer);

	virtual TSharedPtr<ITextDecorator> CreateDecorator(URichTextBlock* InOwner) override;

	UPROPERTY(EditAnywhere, Category = Appearance)
	FHyperlinkStyle m_Style;

	UFUNCTION(BlueprintNativeEvent)
	void ClickFun(const TMap<FString, FString>& metaData, int32 nButtonIndex);

	UPROPERTY(BlueprintAssignable)
		FOnRichTextEvent _bCallBack;

	UPROPERTY(BlueprintAssignable)
	FOnRichTextMapEvent _mapCallBack;

	UPROPERTY()
	TMap<FString, FString>	_mapResutl;
};

class FRichHyperLinkData
{
public:
	FRichHyperLinkData(URichTextLinkBlockDecorator* decorator, TMap<FString, FString> metaData);
	FOnRichTextClickEvent m_LinkDelegate;
};

class FRichInlineHyperLinkDecorator : public FRichTextDecorator
{
public:
	FRichInlineHyperLinkDecorator(URichTextBlock* InOwner, URichTextLinkBlockDecorator* decorator);
	virtual bool Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const override;

protected:
	virtual TSharedPtr<SWidget> CreateDecoratorWidget(const FTextRunInfo& RunInfo, const FTextBlockStyle& TextStyle) const override;

	void ExplodeTextStyleInfo(TMap<FString, FString>& MetaData, FSlateFontInfo& OutFont, FLinearColor& OutFontColor) const;

	URichTextLinkBlockDecorator* m_Decorator;
	mutable FHyperlinkStyle m_Linkstyle;
	mutable FSlateFontInfo m_DefaultFont;
	mutable FSlateColor m_DefaultColor;
	mutable TArray<FRichHyperLinkData> m_LinkData;
	mutable uint8 m_curLinkIndex;
	mutable FString m_Content;
};
//
