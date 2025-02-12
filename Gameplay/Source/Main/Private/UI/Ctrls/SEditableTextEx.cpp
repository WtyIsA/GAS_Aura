#include "SEditableTextEx.h"


SEditableTextEx::SEditableTextEx()
{
	_InputType = EEditInputType::Normal;
}

void SEditableTextEx::OnTextChanged(const FText& InText)
{
	if(!CheckInputText(InText))
	{
		SetText(_oldText);
		return;
	}	
	SEditableText::OnTextChanged(InText);
	_oldText = InText;
}

bool    SEditableTextEx::CheckInputText(const FText& InText)
{
	const FString& str = FTextInspector::GetDisplayString(InText);
	if(str.Len() == 0)
		return true;
	if(_InputType == EEditInputType::Number)
	{
		if(!InText.IsNumeric())
		{
			return  false;
		}
	}
	if(_InputType == EEditInputType::AlphaAndNumber)
	{
		if(!StringIsAlphaNumber(str))
		{
			return false;
		}
	}
	return  true;
}

FReply SEditableTextEx::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if(_OnKeyUp.IsBound())
		_OnKeyUp.Execute(MyGeometry, InKeyEvent);

	return SEditableText::OnKeyUp(MyGeometry, InKeyEvent);
}

void SEditableTextEx::SetOnKeyUpHandler(FOnKeyDown InOnKeyUpHandler)
{
	_OnKeyUp = InOnKeyUpHandler;
}