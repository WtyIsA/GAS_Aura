// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LerpMoveActor.h"

#include "GameFramework/ProjectileMovementComponent.h"

ALerpMoveActor::ALerpMoveActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALerpMoveActor::BeginPlay()
{
	Super::BeginPlay();
	MovementComponent = Cast<UProjectileMovementComponent>(GetComponentByClass(UProjectileMovementComponent::StaticClass()));
	Mesh = Cast<UStaticMeshComponent>(	GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (IsValid(Mesh) && IsValid(MovementComponent))
	{
		MovementComponent->SetInterpolatedComponent(Mesh);
	}
}

void ALerpMoveActor::PostNetReceiveLocationAndRotation()
{
	const FRepMovement& LocalRepMovement = GetReplicatedMovement();
	FVector NewLocation = FRepMovement::RebaseOntoLocalOrigin(LocalRepMovement.Location, this);
	if( RootComponent && RootComponent->IsRegistered() && (NewLocation != GetActorLocation() || LocalRepMovement.Rotation != GetActorRotation()) )
	{
		if (IsValid(MovementComponent))
		{
			MovementComponent->MoveInterpolationTarget(NewLocation,LocalRepMovement.Rotation);
		}
	}
}


void ALerpMoveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

