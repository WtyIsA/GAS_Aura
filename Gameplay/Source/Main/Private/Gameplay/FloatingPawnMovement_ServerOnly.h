// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "FloatingPawnMovement_ServerOnly.generated.h"

/**
 * 
 */
UCLASS()
class MAIN_API UFloatingPawnMovement_ServerOnly : public UFloatingPawnMovement
{
	GENERATED_BODY()
	virtual bool NeedsLoadForClient() const override { return false; }
};
