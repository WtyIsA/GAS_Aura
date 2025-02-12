// Copyright Epic Games, Inc. All Rights Reserved.

#include "SelfBaseJoystick.h"
#include "SSelfBaseJoystick.h"
#include "Slate/DeferredCleanupSlateBrush.h"


USelfBaseJoyStrick::USelfBaseJoyStrick(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void USelfBaseJoyStrick::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	if (MyJoystick.IsValid())
	{
		MyJoystick->ClearControls();
	}
	Activate(MyJoystick);
}

TSharedRef<SWidget> USelfBaseJoyStrick::RebuildWidget()
{
	MyJoystick = SNew(SSelfBaseJoystick);
	Activate(MyJoystick);	
	return  MyJoystick.ToSharedRef();
}

void USelfBaseJoyStrick::Activate(TSharedPtr<SSelfBaseJoystick> VirtualJoystick)
{
	if (VirtualJoystick.IsValid())
	{
		VirtualJoystick->m_pJoyStrick = this;
		VirtualJoystick->SetGlobalParameters(ActiveOpacity, InactiveOpacity, TimeUntilDeactive, TimeUntilReset, ActivationDelay, bPreventRecenter, StartupDelay);
		
		// convert from the UStructs to the slate structs
		TArray<SVirtualJoystick::FControlInfo> SlateControls;

		for (int32 ControlIndex = 0; ControlIndex < Controls.Num(); ControlIndex++)
		{
			FSelfTouchInputControl Control = Controls[ControlIndex];
			SVirtualJoystick::FControlInfo* SlateControl = new(SlateControls)SVirtualJoystick::FControlInfo;

			SlateControl->Image1 = StaticCastSharedRef<ISlateBrushSource>(FDeferredCleanupSlateBrush::CreateBrush(Control.Image1));
			SlateControl->Image2 = StaticCastSharedRef<ISlateBrushSource>(FDeferredCleanupSlateBrush::CreateBrush(Control.Image2));
			SlateControl->Center = Control.Center;
			SlateControl->VisualSize = Control.VisualSize;
			SlateControl->ThumbSize = Control.ThumbSize;
			if (Control.InputScale.SizeSquared() > FMath::Square(DELTA))
			{
				SlateControl->InputScale = Control.InputScale;
			}
			SlateControl->InteractionSize = Control.InteractionSize;
			SlateControl->MainInputKey = Control.MainInputKey;
			SlateControl->AltInputKey = Control.AltInputKey;
		}

		// set them as active!
		VirtualJoystick->SetControls(SlateControls);
	}
}

void USelfBaseJoyStrick::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyJoystick.Reset();
}
void USelfBaseJoyStrick::SetInputScale(float scale)
{
	if(MyJoystick.IsValid())
	{
		MyJoystick->SetInputScale(scale);
	}
}

void USelfBaseJoyStrick::SetActiveOpacity(float fActiveOpacity)
{
	if (MyJoystick.IsValid())
	{
		MyJoystick->SetActiveOpacity(fActiveOpacity);
	}
}