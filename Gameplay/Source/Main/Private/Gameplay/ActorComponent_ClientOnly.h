// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorComponent_ClientOnly.generated.h"


UCLASS(ClassGroup=(Custom),Blueprintable, BlueprintType, meta=(BlueprintSpawnableComponent))
class MAIN_API UActorComponent_ClientOnly : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual bool NeedsLoadForClient() const override { return true; }
	virtual bool NeedsLoadForServer() const override { return false; }
};
