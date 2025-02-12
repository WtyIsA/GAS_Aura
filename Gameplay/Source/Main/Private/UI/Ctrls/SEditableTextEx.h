#pragma once
#include <Widgets/Input/SEditableText.h>
#include "SlateWrapperTypesEx.h"

class MAIN_API SEditableTextEx : public SEditableText
{
public:
    SEditableTextEx();

    EEditInputType  _InputType ;
    virtual void OnTextChanged(const FText& InText) override;
    bool    CheckInputText(const FText& InText);
    FText   _oldText;

    //Onkeyup 和 OnkeyDown参数一样，因此直接用Fonkeydown
    void SetOnKeyUpHandler(FOnKeyDown InOnKeyUpHandler);
    virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent);

private:
    FOnKeyDown  _OnKeyUp;
};
