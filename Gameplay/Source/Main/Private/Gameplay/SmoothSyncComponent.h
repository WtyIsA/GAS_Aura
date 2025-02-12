// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SmoothSyncComponent.generated.h"

class UFootPrintComponent;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class USmoothSyncComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USmoothSyncComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important,
		meta = (ClampMin = "0.01", ClampMax = "1.0", UIMin = "0.01", UIMax = "1.0"))
	float AllowNetDelay = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	float RotationLerpTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	bool bSyncRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated,Category = Important)
	bool bRotationFollowVelocity = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	bool bRotationFollowVelocityIgnoreRoll = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	bool bRotationFollowVelocityIgnorePitch = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	bool bRotationFollowVelocityIgnoreYaw = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	bool bExtrapolation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	float ExtrapolationTime = 0.05;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	bool bUseFixedRotationSpeed = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	float FixedRotationSpeed = 90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStickGround = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Height = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFixSlope = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSweep = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DrawServerPath = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanSetPosImmediately = true;

	UFUNCTION(BlueprintCallable)
	FVector GetVelocity();

	void SendRotation();

	UFUNCTION(NetMulticast, Reliable,BlueprintCallable)
	void ServerSendResetPosToEveryone(FVector pos,bool bLerp = true);

	UFUNCTION(NetMulticast, Reliable,BlueprintCallable)
	void ServerSetRotationFollowVelocity(bool follow);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	USceneComponent* root;
	UMeshComponent* CheckRenderComponent;
	bool bServer = false;
	UFootPrintComponent* FootPrintComponent;
	UFUNCTION(NetMulticast, unreliable)
	void ServerSendPosToEveryone(FVector pos,bool bLerp = true);

	UFUNCTION(NetMulticast, unreliable)
	void ServerSendRotationToEveryone(uint16 roll, uint16 pitch, uint16 yaw);
	UFUNCTION(BlueprintCallable)
	void SetEnable(bool enable);
	UFUNCTION(BlueprintCallable)
    void SetLastReceivedPos(FVector pos);

	static bool SameVector(FVector one, FVector two, float threshold);

	void ClientBeginPlay();
	void ServerBeginPlay();
	
	void ServerTick(float DeltaTime);
	void ClientTick(float DeltaTime);
	
	void SendPos();

	void ClientLerpPos(float DeltaTime);
	void ClientLerpRotation(float DeltaTime);
	void CalculateLerpVelocity();
	void LimitRotation360(FRotator& rotator);
	void RotationFollowVelocity(float DeltaTime);
	
	FVector lastSendPos = FVector::ZeroVector;
	FVector lastSendRotaion = FVector::ZeroVector;
	FVector lastReceivedPos = FVector::ZeroVector;
	FVector lastFramePos = FVector::ZeroVector;
	FVector curVelocity = FVector::ZeroVector;
	FRotator lastReceivedRotation = FRotator::ZeroRotator;

	float lerpPosSpeed = 0;
	float lerpRotatorSpeed = 0;
	
	UPROPERTY(ReplicatedUsing = OnRep_ServerFps)
	float ServerFps = 10;
	UFUNCTION()
	void OnRep_ServerFps();

	float serverTickTime=0;
	bool bReachRecivedPos = false;
	bool bEnable = true;

protected:
	virtual void BeginPlay() override;
};


