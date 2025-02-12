#include "UI/Joystick/FTouchAxisInputPreprocessor.h"


FTouchAxisInputPreprocessor::FTouchAxisInputPreprocessor(FInputPreProcessInterface* pCharacter)
{
	_pitchValue = _yawValue = 0.0f;
	m_Character = pCharacter;
}

bool FTouchAxisInputPreprocessor::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent)
{
	if(InAnalogInputEvent.GetKey().GetFName() == TEXT("Gamepad_RightX"))
	{
		_yawValue = InAnalogInputEvent.GetAnalogValue();		
		if(m_Character)
		{
			return  m_Character->AddYawInput(_yawValue);
		}
		return  true;
	}
	if(InAnalogInputEvent.GetKey().GetFName() == TEXT("Gamepad_RightY"))
	{
		_pitchValue = InAnalogInputEvent.GetAnalogValue();		
		if(m_Character)
		{
			return  m_Character->AddPitchInput(_pitchValue );
		}
		return  true;
	}
	return false;
}