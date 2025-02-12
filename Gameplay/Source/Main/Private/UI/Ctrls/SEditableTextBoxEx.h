#pragma once
#include <Widgets/Input/SEditableTextBox.h>

class MAIN_API SEditableTextBoxEx : public SEditableTextBox
{
public:
    SEditableTextBoxEx();

    //Onkeyup 和 OnkeyDown参数一样，因此直接用Fonkeydown
    void SetOnKeyUpHandler(FOnKeyDown InOnKeyUpHandler);
    virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent);

    private:
    FOnKeyDown  _OnKeyUp;
};
