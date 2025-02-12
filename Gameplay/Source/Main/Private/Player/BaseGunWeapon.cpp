// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/BaseGunWeapon.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MyShootCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABaseGunWeapon::ABaseGunWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

// Called when the game starts or when spawned
void ABaseGunWeapon::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABaseGunWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MyDeltaTime = DeltaTime;
}


void ABaseGunWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


template <class T>
T* ABaseGunWeapon::LoadBlueprint(UClass* u, const TCHAR* path)
{
	UClass* loadObj = StaticLoadClass(u, nullptr, path);
	//UObject* loadObj = StaticLoadObject(u,nullptr,path);
	if (loadObj != nullptr)
	{
		T* ubp = Cast<T>(loadObj);
		if (ubp != nullptr)
		{
			return ubp;
		}
		return nullptr;
	}
	return nullptr;
}

template <class T>
T* ABaseGunWeapon::LoadActorByBlueprint(UClass* u, const TCHAR* path)
{
	UClass* blueprints = StaticLoadClass(u, nullptr, path);
	if (blueprints != nullptr)
	{
		FActorSpawnParameters p;
		p.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		T* actor = GetWorld()->SpawnActor<T>(blueprints, p);
		if (actor != nullptr)
		{
			return actor;
		}
		return nullptr;
	}
	return nullptr;
}
