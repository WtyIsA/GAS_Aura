// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

UENUM(BlueprintType)
enum class EStateType:uint8
{
	Default = 0,
	Stand = 1,
	Crouch = 2,
	Vehicle = 10,
	Swim = 13,
	Ground = 16,
	Climb = 17,
	InAir = 21,
	BackTo = 23,
	Ladder =26,
};
