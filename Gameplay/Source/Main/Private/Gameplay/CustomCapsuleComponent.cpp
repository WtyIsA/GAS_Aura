// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCapsuleComponent.h"

void UCustomCapsuleComponent::InitSweepCollisionParams(FCollisionQueryParams& OutParams,
	FCollisionResponseParams& OutResponseParam) const
{
	Super::InitSweepCollisionParams(OutParams, OutResponseParam);
	OutParams.MobilityType = EQueryMobilityType::Dynamic;
}
