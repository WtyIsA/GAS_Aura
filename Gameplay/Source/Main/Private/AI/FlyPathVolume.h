// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPathVolume.h"
#include "GameFramework/Actor.h"
#include "FlyPathVolume.generated.h"

UCLASS()
class MAIN_API AFlyPathVolume : public ACPathVolume
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlyPathVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
