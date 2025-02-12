// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/AttackTracer.h"
#include "AbilitySystem/CustomAnimNotifyState.h"
#include "UObject/Object.h"
#include "AttackTracerMgr.generated.h"

UCLASS()
class UAttackTracerInfo: public UObject
{

	GENERATED_BODY()
public:
	UPROPERTY()
	USkeletalMeshComponent* Weapon;
	IWeaponOwnerInterface* Attacker;
	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;
	UPROPERTY()
	TArray<FHitResult> HitResults;
	UPROPERTY()
	TSet<AActor*> HitActors;
	UPROPERTY()
	TArray<FVector> LastLocations;
	TArray<FName> SocketNames;
	
};
UCLASS()
class MAIN_API UAttackTracerMgr : public UObject
{
	GENERATED_BODY()

public:
	static UAttackTracerMgr* Get();
	static UAttackTracerMgr* MgrHandle;
	UPROPERTY()
	TSet<USkeletalMeshComponent*> InBounce;
	UPROPERTY()
	TSet<USkeletalMeshComponent*> InStuck;
	UPROPERTY()
	TMap<USkeletalMeshComponent*,UAttackTracerInfo*> AttackTracerInfoMap;
	UFUNCTION()
	void AddBlockStuck(USkeletalMeshComponent* Mesh);
	UFUNCTION()
	void RemoveBlockStuck(USkeletalMeshComponent* Mesh);
	UAttackTracerInfo& GetAttackTracerInfo(USkeletalMeshComponent* mesh);
	UFUNCTION()
	void BeginStuck(float StuckTime,float StuckPlayRate,USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Anim);
	UFUNCTION()
	void BeginBounce(float BounceTime,float BouncePlayRate,float BounceBlendOutTime,USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Anim);
};
