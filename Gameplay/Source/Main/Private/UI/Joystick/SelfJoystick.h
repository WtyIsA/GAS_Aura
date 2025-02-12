// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


//#include "AssetTypeActions_Base.h"
#include "SelfBaseJoystick.h"
#include "SSelfJoystick.h"
#include "Components/Widget.h"

#include "SelfJoystick.generated.h"


UCLASS(config = Editor, defaultconfig)
class USelfJoyStrick : public  USelfBaseJoyStrick
{
	GENERATED_UCLASS_BODY()

	
	UPROPERTY(EditAnywhere, Category="TouchInterface")
	FMargin ShowRunMargin;
	UPROPERTY(EditAnywhere, Category="TouchInterface")
	FMargin RunCtrlRect;
		
	UPROPERTY(EditAnywhere, Category="TouchInterface")
	float rushDisASqr = 10000.0f;
	
	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnShowRun,  bool, ShowRun);

	UPROPERTY(EditAnywhere, Category="TouchInterface")
	FOnShowRun OnShowRunEvent;
	UPROPERTY(EditAnywhere, Category="TouchInterface")
	FOnShowRun OnRunMouseUpEvent;

	UPROPERTY(EditAnywhere, Category="TouchInterface")
	FOnShowRun OnJoystickStart;
	UPROPERTY(EditAnywhere, Category="TouchInterface")
	FOnShowRun OnJoystickEnd;
	UPROPERTY(EditAnywhere, Category="TouchInterface")
	FOnShowRun OnRush;
public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void Activate(TSharedPtr<SSelfBaseJoystick> VirtualJoystick);
	virtual void SynchronizeProperties() override;
	void NativeShowRun(bool bShowRun);
	void NativeRunMouseUp(bool bShowRun);
	void NativeJoystickStart(bool bShowRun);
	void NativeJoystickEnd(bool bEnd);
	void NativeRush(bool bRush);
	

	UFUNCTION()
	void SetThumbBarFix(bool bFix);
	
	UFUNCTION()
	void SetShowAutoRush(bool bRush);

	UFUNCTION()
	FVector2D GetThumbPos(int32 ControlIndex);
	
	UFUNCTION()
    void ReleaseJoystick();
	
private:
	TSharedPtr<SSelfJoystick> MyJoystick;
};
