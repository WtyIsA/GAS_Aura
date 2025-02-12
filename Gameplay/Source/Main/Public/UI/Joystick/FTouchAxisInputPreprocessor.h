#pragma once
#include "Framework/Application/IInputProcessor.h"

class FInputPreProcessInterface
{
public:
	FInputPreProcessInterface(){};

	virtual bool AddYawInput(float value) = 0;
	virtual bool AddPitchInput(float value) = 0;
};

class FTouchAxisInputPreprocessor : public IInputProcessor
{
public:
	FTouchAxisInputPreprocessor(FInputPreProcessInterface* pCharacter);
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor){};
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent);

private:
	float  _yawValue;
	float  _pitchValue;
	FInputPreProcessInterface*	m_Character;
};
