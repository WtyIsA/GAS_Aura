// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "BaseWeapon.h"
// #include "MatineeCameraShake.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "BaseGunWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType:uint8 
{
	Rifle,
	Pistol,
	Knife
};
class AMyShootCharacter;
class ABaseScope;
UCLASS()
class  ABaseGunWeapon : public ABaseWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseGunWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float MyDeltaTime;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	// TSubclassOf<ULegacyCameraShake> CameraShake;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	template<class T>
    T* LoadBlueprint(UClass* u, const TCHAR* path);
	template<class T>
    T* LoadActorByBlueprint(UClass*,const TCHAR* path);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int WeaponType =1;//1 hand gun 2 rifle
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float DisFromCamera =0;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bHaveScope;
	//FTransform GetAimPoint();
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FVector4 GunFireRecede;
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FRotator GunFireRotator;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float AdsFov;

};
