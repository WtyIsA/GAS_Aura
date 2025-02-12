#include "Text/TextFormatArgumentModifier_Main.h"

#include "LuaManger.h"
#include "UnLuaBase.h"
#include "UnLua/Public/LuaMessageTableOptInC.h"

TSharedPtr<ITextFormatArgumentModifier> TextFormatArgumentModifier_FromConfig::Create(const FTextFormatString& InArgsString)
{
	FString FunctionArg = InArgsString.StringPtr;
	int32 BracketIndex = FunctionArg.Find(")");
	FunctionArg.RemoveAt(BracketIndex,FunctionArg.Len()-BracketIndex);
	return MakeShareable(new TextFormatArgumentModifier_FromConfig(FunctionArg));
}

bool TextFormatArgumentModifier_FromConfig::Validate(const FCultureRef& InCulture, TArray<FString>& OutValidationErrors) const
{
	return true;
}

void TextFormatArgumentModifier_FromConfig::Evaluate(const FFormatArgumentValue& InValue, const FPrivateTextFormatArguments& InFormatArgs, FString& OutResult) const
{
	if (!LuaParameter.IsEmpty())
	{
		if (InValue.GetType() == EFormatArgumentType::Int)
		{
			RefreshCacheString(InValue.GetIntValue());
		}
		else if (InValue.GetType() == EFormatArgumentType::UInt)
		{
			RefreshCacheString(InValue.GetUIntValue());
		}
		else if (InValue.GetType() == EFormatArgumentType::Double)
		{
			RefreshCacheString(InValue.GetDoubleValue());
		}
		else if (InValue.GetType() == EFormatArgumentType::Float)
		{
			RefreshCacheString(InValue.GetFloatValue());
		}
		else if (InValue.GetType() == EFormatArgumentType::Text)
		{
			FString inValueStr = InValue.GetTextValue().ToString();
			if (inValueStr.IsNumeric())
			{
				RefreshCacheString(FCString::Atoi(*inValueStr));
			}
			else
			{
				OutResult += inValueStr;
			}
		}
		else
		{
			return;
		}
		OutResult += CacheTranslatedString;
	}
}

void TextFormatArgumentModifier_FromConfig::GetFormatArgumentNames(TArray<FString>& OutArgumentNames) const
{
}

void TextFormatArgumentModifier_FromConfig::EstimateLength(int32& OutLength, bool& OutUsesFormatArgs) const
{
	OutUsesFormatArgs = false;
	if (CacheTranslatedString.IsEmpty())
	{
		OutLength = WantedLength;
	}
	else
	{
		OutLength = CacheTranslatedString.Len();
	}
}

TextFormatArgumentModifier_FromConfig::TextFormatArgumentModifier_FromConfig(FString& InInLuaParameter)
	:LuaParameter(InInLuaParameter)
{
	WantedLength = InInLuaParameter.Contains("desc") ? 100:10;
}

void TextFormatArgumentModifier_FromConfig::RefreshCacheString(int32 CfgId) const
{
	if(LuaParameter.IsEmpty())
	{
		UE_LOG(GAME,Error,TEXT("[TextFormatArgumentModifier_FromConfig]GetStringFromConfig,LuaParameter is empty.LuaParameter : %s"),
				*LuaParameter)
		return;
	}

	const FString FunctionTable = TEXT("TextFormatArgumentModifierLuaTable");
	const FString FunctionName = TEXT("FromConfig");
	lua_State *L = UnLua::GetState();
	lua_getglobal(L,TCHAR_TO_ANSI(*FunctionTable));
	lua_getfield(L, -1, TCHAR_TO_ANSI(*FunctionName));
	lua_pushinteger(L, CfgId);
	lua_pushstring(L,TCHAR_TO_ANSI(*LuaParameter));
	if(lua_pcall(L,2,1,0) != LUA_OK)
	{
		UE_LOG(GAME,Error,TEXT("[TextFormatArgumentModifier_FromConfig]Call function error.CfgId : %d, LuaParameter : %s"),
				CfgId,
				*LuaParameter)
		lua_pop(L,1);
		return;
	}
	FString* TranslatedString = const_cast<FString*>(&CacheTranslatedString);
	FString LuaReturnValue = UTF8_TO_TCHAR(lua_tostring(L,-1));
	*TranslatedString = (ULuaManger::Get()->bNeedTranslation ? (ULuaManger::Get()->GetStringWithTranslation(LuaReturnValue).ToString()) : LuaReturnValue);
	lua_pop(L,1);
}