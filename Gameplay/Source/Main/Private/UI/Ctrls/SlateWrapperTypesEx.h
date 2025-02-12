#pragma once

UENUM(BlueprintType)
enum class EEditInputType : uint8
{
	/*can input any character*/
	Normal,
	/*Can only input numbers*/
    Number,
	/* Can  input letters and numbers*/
    AlphaAndNumber
};

extern bool StringIsAlphaNumber(const FString& str);

//判断一个字符是否为字母，调用系统的isalpha,判断汉字时出错
extern bool IsAlphaEx(TCHAR c);