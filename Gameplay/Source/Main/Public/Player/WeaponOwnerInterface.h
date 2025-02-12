﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponOwnerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UWeaponOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MAIN_API IWeaponOwnerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual USkeletalMeshComponent* GetRightWeaponMesh() const = 0;
	virtual USkeletalMeshComponent* GetLeftWeaponMesh() const = 0;
	virtual  void OnNearWeaponHit(const FHitResult& HitResult , const FName& HitSocketName) const = 0;
};
