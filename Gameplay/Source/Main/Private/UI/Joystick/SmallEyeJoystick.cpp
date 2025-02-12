// Copyright Epic Games, Inc. All Rights Reserved.

#include "SmallEyeJoystick.h"
#include "Slate/DeferredCleanupSlateBrush.h"


USmallEyeJoyStrick::USmallEyeJoyStrick(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void USmallEyeJoyStrick::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (MyJoystick.IsValid())
	{
		MyJoystick->ClearControls();
	}
	Activate(MyJoystick);	
}

TSharedRef<SWidget> USmallEyeJoyStrick::RebuildWidget()
{
	MyJoystick = SNew(SSmallEyeJoyStrick);
	USelfBaseJoyStrick::MyJoystick  = MyJoystick;
	Activate(MyJoystick);
	return  MyJoystick.ToSharedRef();
}

void USmallEyeJoyStrick::Activate(TSharedPtr<SSelfBaseJoystick> VirtualJoystick)
{
	USelfBaseJoyStrick::Activate(VirtualJoystick);
	if (VirtualJoystick.IsValid())
	{
		SSmallEyeJoyStrick* pSmallEyey = (SSmallEyeJoyStrick*)(VirtualJoystick.Get());		
		pSmallEyey->SetDeadZone(DeadZone);
	}
}

void USmallEyeJoyStrick::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyJoystick.Reset();
}


void USmallEyeJoyStrick::NativeJoystickStart(bool bStart)
{
	if(OnJoystickStart.IsBound())
		OnJoystickStart.Execute(bStart);
}

void USmallEyeJoyStrick::NativeJoystickEnd(bool bEnd)
{
	if(OnJoystickEnd.IsBound())
		OnJoystickEnd.Execute(bEnd);
}

void USmallEyeJoyStrick::ReleaseJoystick()
{
	if (MyJoystick.IsValid())
	{
		MyJoystick->ReleaseJoystick();
	}
}

void USmallEyeJoyStrick::SetTurnValue(float fInDeadZone, float fInInitValue, float fInPitchInputScale)
{
	if (MyJoystick.IsValid())
	{
		MyJoystick->SetTurnValue(fInDeadZone,fInInitValue,fInPitchInputScale);
	}
}