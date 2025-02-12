// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


//#include "AssetTypeActions_Base.h"
#include "Components/Widget.h"

#include "TouchTurn.generated.h"

class STouchTurn;


UCLASS(config = Editor, defaultconfig)
class UTouchTurn : public  UWidget
{
	GENERATED_UCLASS_BODY()
	UPROPERTY(EditAnywhere, Category="Scale")
	float YawScale = 10.0f;
	UPROPERTY(EditAnywhere, Category="Scale")
	float PitchScale = 10.0f;

	UPROPERTY(EditAnywhere, Category="Scale")
	float SpeedUpLimitPixel = 25;
	UPROPERTY(EditAnywhere, Category="Scale")
	float InitValue = 18;
	UPROPERTY(EditAnywhere, Category="Scale")
	float SlopeValue = 0.1;
	UPROPERTY(EditAnywhere, Category="Scale")
	float InputPitchScale = 0.7;

	UPROPERTY(EditAnywhere, Category="Scale")
	bool bIgnoreTextureSize = false;

	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnPointerEvent OnMouseButtonDownEvent;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FOnPointerEvent OnMouseButtonUpEvent;
	
public:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void SynchronizeProperties() override;
	TSharedPtr<STouchTurn> MyTouchTurn;
	UFUNCTION()
	void SetTurnValue(float i, float s,float limitPixel);
	UFUNCTION()
	void SetScopeValue(float s);
	UFUNCTION()
	void SetPitchValue(float pitchvalue);
	UFUNCTION()
	void SetInitValue(float value);
	UFUNCTION()
	void SetInputScale(float fYawscale, float fPitchscale);

private:
	FReply HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
	FReply HandleMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent);
};


