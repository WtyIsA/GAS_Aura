// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/EStateType.h"
#include "BaseCheatChecker.generated.h"


enum class EStateType : uint8;
class USyncMoveComponent;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAIN_API UBaseCheatChecker : public UActorComponent
{
	GENERATED_BODY()

public:
	UBaseCheatChecker();
	USyncMoveComponent* SyncMoveComponent;
	FVector enterAirVelocity;
	FVector enterAirPos;
	FVector startCurveMovePos;
	double enterAirTimeStamp;
	double startCurveMoveTimeStamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	float MaxSpeed = 650;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	float MaxUpSpeed = 550;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	float RootHeight = 92;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	float GravityScale = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	float EnterSwimDepth = 130;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	bool bCheckSwim = true;
	
protected:
	virtual void BeginPlay() override;
	TArray<AActor*> IgnoreActors;
	const UPrimitiveComponent* PrimitiveComponent;
	EStateType curState = EStateType::Ground;
	EStateType lastState = EStateType::Ground;
	UCurveFloat* xMoveCurve;
	UCurveFloat* yMoveCurve;
	UCurveFloat* zMoveCurve;
public:
	void SetCurState(EStateType state);
	virtual	bool IsTargetPosFly(FVector targetPos,EStateType state);
	virtual void SelfPullBacked();
	bool IsAirPosOk(FVector targetPos, double timestamp,EStateType state);
	virtual	bool IsCollisionRadiusChange();
	virtual	bool IsClimbPosOk(FVector targetPos,EStateType state);
	virtual	bool IsLadderPosOk(FVector targetPos,EStateType state);
	virtual	bool IsCurvePosOk(FVector targetPos,EStateType state, double timestamp);
	FVector GetFlyPos(const FVector& StartPos, const FVector& Speed, const double FlyTime) const;
	virtual FVector GetGroundPos(FVector pos,bool& hasGround);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual bool PassSwimStateCheck(EStateType state);
	UFUNCTION(BlueprintCallable)
	void SetMaxSpeed(float speed);
	UFUNCTION(BlueprintCallable)
	void SetXMoveCurve(UCurveFloat* curve);
	UFUNCTION(BlueprintCallable)
	void SetYMoveCurve(UCurveFloat* curve);
	UFUNCTION(BlueprintCallable)
	void SetZMoveCurve(UCurveFloat* curve);
};
