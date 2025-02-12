// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


//#include "AssetTypeActions_Base.h"
#include "SelfBaseJoystick.h"
#include "SSelfJoystick.h"
#include "SSmallEyeJoystick.h"
#include "Components/Widget.h"

#include "SmallEyeJoystick.generated.h"


UCLASS(config = Editor, defaultconfig)
class USmallEyeJoyStrick : public  USelfBaseJoyStrick
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(EditAnywhere, Category="TouchInterface")
	float DeadZone = 0.0f;
	
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnShowRun,  bool, ShowRun);

	UPROPERTY(EditAnywhere, Category="TouchInterface")
	FOnShowRun OnJoystickStart;
	UPROPERTY(EditAnywhere, Category="TouchInterface")
	FOnShowRun OnJoystickEnd;
	
public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void Activate(TSharedPtr<SSelfBaseJoystick> VirtualJoystick);
	virtual void SynchronizeProperties() override;
	
	void NativeJoystickStart(bool bShowRun);
	void NativeJoystickEnd(bool bEnd);
	UFUNCTION()
	void ReleaseJoystick();

	UFUNCTION()
	void SetTurnValue(float fInDeadZone, float fInInitValue, float fInPitchInputScale);
	
private:
	TSharedPtr<SSmallEyeJoyStrick> MyJoystick;
};
