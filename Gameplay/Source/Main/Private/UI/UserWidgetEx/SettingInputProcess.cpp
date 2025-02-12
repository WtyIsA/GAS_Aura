#include "SettingInputProcess.h"

bool FSettingInputProcess::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	ULuaManger::Get()->OnPressKey(InKeyEvent.GetKey().GetFName().ToString());
	ULuaManger::Get()->UnregisterSettingInputProcess();
	return true;
}

bool FSettingInputProcess::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	ULuaManger::Get()->OnPressKey(MouseEvent.GetEffectingButton().GetFName().ToString());
	ULuaManger::Get()->UnregisterSettingInputProcess();
	return true;
}

bool FSettingInputProcess::HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp,
	const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent)
{
	ULuaManger::Get()->OnPressKey(InWheelEvent.GetEffectingButton().GetFName().ToString());
	ULuaManger::Get()->UnregisterSettingInputProcess();
	return true;
}

void FSettingInputProcess::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
}
