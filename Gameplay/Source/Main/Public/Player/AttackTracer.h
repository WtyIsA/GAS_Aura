// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponOwnerInterface.h"
#include "AbilitySystem/CustomAnimNotifyState.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AttackTracer.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTracerHittedDelegate,const AActor* ,Owner,const FHitResult& ,HitResult, const FName&,SocketName);

UCLASS()
class MAIN_API UAttackTracer : public UCustomAnimNotifyState
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TMap<FName,FVector> SocketName2HitBoxMap;
	UPROPERTY(EditAnywhere)
	bool bRight = true;
	UPROPERTY(EditAnywhere)
	bool bLeft = false;
	UPROPERTY(EditAnywhere)
	FName TraceProfileName;
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugTrace = EDrawDebugTrace::None;

	void ProcessHits(const TArray<FHitResult>& HitResults, const FName& HitSocketName,USkeletalMeshComponent* MeshComp);

public:
	UPROPERTY(EditAnywhere)
	bool bOpenStuck = true;
	UPROPERTY(EditAnywhere)
	float StuckTime = 0.2;
	UPROPERTY(EditAnywhere)
	float StuckPlayRate = 0.2;
	UPROPERTY(EditAnywhere)
	bool bOpenBound = true;
	UPROPERTY(EditAnywhere)
	float BounceTime = 0.4;
	UPROPERTY(EditAnywhere)
	float BouncePlayRate = 0.5;
	UPROPERTY(EditAnywhere)
	float BounceBlendOutTime = 0.5;
	UPROPERTY(BlueprintReadOnly)
	FHitResult CurHitResult;
	UPROPERTY(BlueprintReadOnly)
	FName CurHitSocketName;
	UPROPERTY(BlueprintAssignable)
	FTracerHittedDelegate TracerHittedEvent;
	UFUNCTION(BlueprintImplementableEvent)
	void OnHitted(const FHitResult& HitResultArray, const FName& HitSocketName) const;
	
	UFUNCTION(BlueprintCallable)
	bool CanStuck(const FHitResult& HitResult, USkeletalMeshComponent* MeshComp);
	UFUNCTION(BlueprintCallable)
	bool CanBounce(const FHitResult& HitResult, USkeletalMeshComponent* MeshComp);
	UFUNCTION(BlueprintCallable)
	void BeginStuck(USkeletalMeshComponent* MeshComp);
	UFUNCTION(BlueprintCallable)
	void BeginBounce(USkeletalMeshComponent* MeshComp);

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                        float FrameDeltaTime) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                         float TotalDuration) override;
	bool IsServerPlayer();
};
