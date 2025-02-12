// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LerpMoveActor.generated.h"

UCLASS()
class MAIN_API ALerpMoveActor : public AActor
{
	GENERATED_BODY()
	class UProjectileMovementComponent* MovementComponent;
	class UStaticMeshComponent* Mesh;
public:	
	// Sets default values for this actor's properties
	ALerpMoveActor();
	virtual void PostNetReceiveLocationAndRotation() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
