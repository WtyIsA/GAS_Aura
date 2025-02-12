// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#if USER_AKAUDIO

#include "CoreMinimal.h"
#include "Sound/ISlateSoundDevice.h"

#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#include "Editor/UnrealEdEngine.h"
#else
#include "Engine/GameEngine.h"
#endif

struct FSlateSound;

class FSoundDevice : public ISlateSoundDevice
{
private:
	const FName WorldHandle;

public:
	FSoundDevice() 
		:WorldHandle(ULuaManger::Get()->GetCurrentWorldContextHandle())
	{
	};
	virtual ~FSoundDevice() {};

private:
	virtual void PlaySound(const FSlateSound& Sound, int32 UserIndex = 0) const override;
	virtual float GetSoundDuration(const FSlateSound& Sound) const override;

};

#endif