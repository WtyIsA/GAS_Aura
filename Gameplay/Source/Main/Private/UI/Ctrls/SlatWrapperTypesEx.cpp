#include <locale.h>

#include "SlateWrapperTypesEx.h"

bool StringIsAlphaNumber(const FString& str)
{
	if(str.Len() == 0)
		return true;
	const TCHAR* Str = str.GetCharArray().GetData();
	while (*Str != '\0')
	{
		if (!FChar::IsDigit(*Str) && !IsAlphaEx(*Str))
		{
			return false;
		}			
		++Str;
	}
	return  true;
}

bool IsAlphaEx(TCHAR ch)
{
	if((ch>= TEXT('a')&&ch<= TEXT('z'))||(ch>= TEXT('A')&&ch<= TEXT('Z')))
		return true;
	else
		return  false;
}