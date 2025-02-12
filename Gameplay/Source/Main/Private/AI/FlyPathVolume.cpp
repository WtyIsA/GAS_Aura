// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyPathVolume.h"
#include "LuaManger.h"

// Sets default values
AFlyPathVolume::AFlyPathVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFlyPathVolume::BeginPlay()
{
	Super::BeginPlay();
	ULuaManger::Get()->AddFlyPathVolume(this);
}

// Called every frame
void AFlyPathVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
