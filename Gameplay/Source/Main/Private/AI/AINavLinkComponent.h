// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavLinkCustomComponent.h"
#include "AINavLinkComponent.generated.h"


UCLASS(ClassGroup=(Navigation), meta=(BlueprintSpawnableComponent))
class MAIN_API UAINavLinkComponent : public UNavLinkCustomComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetNavEnabled(bool bNewEnabled);
};
