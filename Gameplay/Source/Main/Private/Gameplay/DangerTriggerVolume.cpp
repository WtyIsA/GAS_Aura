// Fill out your copyright notice in the Description page of Project Settings.


#include "DangerTriggerVolume.h"


// Sets default values
ADangerTriggerVolume::ADangerTriggerVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	OnActorBeginOverlap.AddDynamic(this,&ADangerTriggerVolume::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this,&ADangerTriggerVolume::OnOverlapEnd);
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}
void ADangerTriggerVolume::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	Enter(OtherActor);
}

void ADangerTriggerVolume::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	Exit(OtherActor);
}

void ADangerTriggerVolume::Enter_Implementation(AActor* OtherActor)
{
}

void ADangerTriggerVolume::Exit_Implementation(AActor* OtherActor)
{
}

// Called when the game starts or when spawned
void ADangerTriggerVolume::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADangerTriggerVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

