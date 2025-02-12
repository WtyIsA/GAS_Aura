// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapActor.h"


// Sets default values
ATrapActor::ATrapActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

USkeletalMeshComponent* ATrapActor::GetRightWeaponMesh() const
{
	return GetRightHandHoldWeaponMesh();
}

USkeletalMeshComponent* ATrapActor::GetLeftWeaponMesh() const
{
	return GetLeftHandHoldWeaponMesh();
}

void ATrapActor::OnNearWeaponHit(const FHitResult& HitResult, const FName& HitSocketName) const
{
	OnWeaponHitEvent.Broadcast(HitResult, HitSocketName);
}

// Called when the game starts or when spawned
void ATrapActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATrapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

