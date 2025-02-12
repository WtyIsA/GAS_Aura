// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


#include "Components/Widget.h"

#include "SelfBaseJoystick.generated.h"

class SSelfBaseJoystick;


USTRUCT()
struct FSelfTouchInputControl
{
	GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, Category="Control", meta=(ToolTip="For sticks, this is the Thumb"))
	FSlateBrush Image1;
	UPROPERTY(EditAnywhere, Category="Control", meta=(ToolTip="For sticks, this is the Background"))
	FSlateBrush Image2;
	UPROPERTY(EditAnywhere, Category="Control", meta=(ToolTip="The center point of the control (if <= 1.0, it's relative to screen, > 1.0 is absolute)"))
	FVector2D Center;
	UPROPERTY(EditAnywhere, Category="Control", meta=(ToolTip="The size of the control (if <= 1.0, it's relative to screen, > 1.0 is absolute)"))
	FVector2D VisualSize;
	UPROPERTY(EditAnywhere, Category="Control", meta=(ToolTip="For sticks, the size of the thumb (if <= 1.0, it's relative to screen, > 1.0 is absolute)"))
	FVector2D ThumbSize;
	UPROPERTY(EditAnywhere, Category="Control", meta=(ToolTip="The interactive size of the control (if <= 1.0, it's relative to screen, > 1.0 is absolute)"))
	FVector2D InteractionSize;
	UPROPERTY(EditAnywhere, Category = "Control", meta = (ToolTip = "The scale for control input"))
	FVector2D InputScale;
	UPROPERTY(EditAnywhere, Category="Control", meta=(ToolTip="The main input to send from this control (for sticks, this is the horizontal axis)"))
	FKey MainInputKey;
	UPROPERTY(EditAnywhere, Category="Control", meta=(ToolTip="The alternate input to send from this control (for sticks, this is the vertical axis)"))
	FKey AltInputKey;

	FSelfTouchInputControl()
        : Center(ForceInitToZero)
        , VisualSize(ForceInitToZero)
        , ThumbSize(ForceInitToZero)
        , InteractionSize(ForceInitToZero)
        , InputScale(1.f, 1.f)
	{
	}
};

UCLASS()
class USelfBaseJoyStrick : public  UWidget
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category="TouchInterface")
	TArray<FSelfTouchInputControl> Controls;

	UPROPERTY(EditAnywhere, Category="TouchInterface", meta=(ToolTip="Opacity (0.0 - 1.0) of all controls while any control is active"))
	float ActiveOpacity = 1.0;

	UPROPERTY(EditAnywhere, Category="TouchInterface", meta=(ToolTip="Opacity (0.0 - 1.0) of all controls while no controls are active"))
	float InactiveOpacity = 1.0;

	UPROPERTY(EditAnywhere, Category="TouchInterface", meta=(ToolTip="How long after user interaction will all controls fade out to Inactive Opacity"))
	float TimeUntilDeactive = 0.5;

	UPROPERTY(EditAnywhere, Category="TouchInterface", meta=(ToolTip="How long after going inactive will controls reset/recenter themselves (0.0 will disable this feature)"))
	float TimeUntilReset = 0.0;

	UPROPERTY(EditAnywhere, Category="TouchInterface", meta=(ToolTip="How long after joystick enabled for touch (0.0 will disable this feature)"))
	float ActivationDelay = 0.0;

	UPROPERTY(EditAnywhere, Category="TouchInterface", meta=(ToolTip="Whether to prevent joystick re-center"))
	bool bPreventRecenter = true;

	UPROPERTY(EditAnywhere, Category = "TouchInterface", meta = (ToolTip = "Delay at startup before virtual joystick is drawn"))
	float StartupDelay = 0.0;

	
	UPROPERTY(EditAnywhere, Category="TouchInterface")
	float inputScale = 1.0f;
	
	
public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void Activate(TSharedPtr<SSelfBaseJoystick> VirtualJoystick);
	virtual void SynchronizeProperties() override;
	UFUNCTION()
    void SetInputScale(float scale);
	
	TSharedPtr<SSelfBaseJoystick> MyJoystick;


	UFUNCTION()
	void SetActiveOpacity(float fActiveOpacity);
};
