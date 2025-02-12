// Fill out your copyright notice in the Description page of Project Settings.
#include "VehicleCheatChecker.h"

#include "Kismet/KismetSystemLibrary.h"

UVehicleCheatChecker::UVehicleCheatChecker()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UVehicleCheatChecker::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UVehicleCheatChecker::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


