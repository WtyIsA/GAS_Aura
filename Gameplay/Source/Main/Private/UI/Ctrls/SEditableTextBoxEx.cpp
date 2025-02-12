#include "SEditableTextBoxEx.h"


SEditableTextBoxEx::SEditableTextBoxEx()
{
}

void SEditableTextBoxEx::SetOnKeyUpHandler(FOnKeyDown InOnKeyUpHandler)
{
	_OnKeyUp = InOnKeyUpHandler;
}

FReply SEditableTextBoxEx::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if(_OnKeyUp.IsBound())
		return _OnKeyUp.Execute(MyGeometry, InKeyEvent);
	return FReply::Unhandled();
}
