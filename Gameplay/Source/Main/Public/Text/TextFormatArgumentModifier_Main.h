#pragma once

#include "CoreTypes.h"
#include "Containers/UnrealString.h"
#include "Internationalization/Text.h"
#include "Internationalization/ITextFormatArgumentModifier.h"

struct FPrivateTextFormatArguments;

class TextFormatArgumentModifier_FromConfig: public ITextFormatArgumentModifier
{
public:
	static TSharedPtr<ITextFormatArgumentModifier> Create(const FTextFormatString& InArgsString);

	virtual bool Validate(const FCultureRef& InCulture, TArray<FString>& OutValidationErrors) const override;

	virtual void Evaluate(const FFormatArgumentValue& InValue, const FPrivateTextFormatArguments& InFormatArgs, FString& OutResult) const override;

	virtual void GetFormatArgumentNames(TArray<FString>& OutArgumentNames) const override;

	virtual void EstimateLength(int32& OutLength, bool& OutUsesFormatArgs) const override;

private:

	TextFormatArgumentModifier_FromConfig(FString& InLuaParameter);

	void RefreshCacheString(int32 CfgId) const;

	int32 WantedLength;
	FString LuaParameter;
	FString CacheTranslatedString;
};
