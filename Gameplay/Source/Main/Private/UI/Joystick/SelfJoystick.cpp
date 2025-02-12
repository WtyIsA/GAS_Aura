// Copyright Epic Games, Inc. All Rights Reserved.

#include "SelfJoystick.h"
#include "SSelfJoystick.h"
#include "Slate/DeferredCleanupSlateBrush.h"


USelfJoyStrick::USelfJoyStrick(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void USelfJoyStrick::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (MyJoystick.IsValid())
	{
		MyJoystick->ClearControls();
	}
	Activate(MyJoystick);	
}

TSharedRef<SWidget> USelfJoyStrick::RebuildWidget()
{
	MyJoystick = SNew(SSelfJoystick);
	USelfBaseJoyStrick::MyJoystick  = MyJoystick;
	Activate(MyJoystick);		
	return  MyJoystick.ToSharedRef();
}

void USelfJoyStrick::Activate(TSharedPtr<SSelfBaseJoystick> VirtualJoystick)
{
	USelfBaseJoyStrick::Activate(VirtualJoystick);
	if (VirtualJoystick.IsValid())
	{
		SSelfJoystick* pSelfJoystrick = (SSelfJoystick*)(VirtualJoystick.Get());
		pSelfJoystrick->SetShowRunMargin(ShowRunMargin);
		pSelfJoystrick->SetRunCtrlRect(RunCtrlRect);
		pSelfJoystrick->SetRushDisAspr(rushDisASqr);
	}
}

void USelfJoyStrick::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyJoystick.Reset();
}

void USelfJoyStrick::SetThumbBarFix(bool bFix)
{
	
	if(MyJoystick.IsValid())
	{
		SSelfJoystick* pSelfJoyStick = (SSelfJoystick*)MyJoystick.Get();
		if(pSelfJoyStick != nullptr)
			pSelfJoyStick->SetThumbBarFix(bFix);
	}
}




void USelfJoyStrick::NativeShowRun(bool bShowRun)
{
	if(OnShowRunEvent.IsBound())
		OnShowRunEvent.Execute(bShowRun);
}

void USelfJoyStrick::NativeRunMouseUp(bool bUp)
{
	if(OnRunMouseUpEvent.IsBound())
		OnRunMouseUpEvent.Execute(bUp);
}

void USelfJoyStrick::NativeJoystickStart(bool bStart)
{
	if(OnJoystickStart.IsBound())
		OnJoystickStart.Execute(bStart);
}

void USelfJoyStrick::NativeJoystickEnd(bool bEnd)
{
	if(OnJoystickEnd.IsBound())
		OnJoystickEnd.Execute(bEnd);
}

void USelfJoyStrick::NativeRush(bool bRush)
{
	if(OnRush.IsBound())
		OnRush.Execute(bRush);
}

void USelfJoyStrick::SetShowAutoRush(bool bRush)
{
	if(MyJoystick.IsValid())
	{
		MyJoystick->SetAutoRush(bRush);
	}
}

FVector2D USelfJoyStrick::GetThumbPos(int32 ControlIndex)
{
	if(MyJoystick.IsValid())
	{
		return MyJoystick->GetThumbPos(ControlIndex);
	}
	return FVector2D(0,0);
}

void USelfJoyStrick::ReleaseJoystick()
{
	if(MyJoystick.IsValid())
	{
		SSelfJoystick* pSelfJoyStick = (SSelfJoystick*)MyJoystick.Get();
		if(pSelfJoyStick != nullptr)
			pSelfJoyStick->ReleaseJoystick();
	}
}