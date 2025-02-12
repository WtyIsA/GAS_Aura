// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Player/WeaponOwnerInterface.h"
#include "TrapActor.generated.h"

UCLASS()
class MAIN_API ATrapActor : public AActor,public  IWeaponOwnerInterface
{
	GENERATED_BODY()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponHit, FHitResult, hitInfo, FName,HitSocketName);
public:
	// Sets default values for this actor's properties
	ATrapActor();
	UFUNCTION(BlueprintImplementableEvent)
    USkeletalMeshComponent* GetRightHandHoldWeaponMesh() const;
	UFUNCTION(BlueprintImplementableEvent)
	USkeletalMeshComponent* GetLeftHandHoldWeaponMesh() const;
	virtual USkeletalMeshComponent* GetRightWeaponMesh() const override;
	virtual USkeletalMeshComponent* GetLeftWeaponMesh() const override;
	UPROPERTY(BlueprintAssignable)
	FOnWeaponHit OnWeaponHitEvent;
	virtual void OnNearWeaponHit(const FHitResult& HitResult, const FName& HitSocketName) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
