// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomSignificanceInterface.h"
#include "SyncMoveComponent.h"
#include "GameFramework/Character.h"
#include "Tasks/AITask_MoveTo.h"
#include "ARider.generated.h"

UCLASS()
class MAIN_API AARider : public ACharacter,public FCustomSignificanceInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AARider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual  void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	USyncMoveComponent* SyncMoveComponent;
public:
	UFUNCTION(BlueprintCallable)
	void UpdateSignificanceSettings(const TMap<float, float>& SignificanceThresholdsMap);
	virtual float SignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo,
	                                   const FTransform& Viewpoint) override;
	virtual void PostSignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance,
	                                      float Significance, bool bFinal) override;
	void SetTickInterval(float interval);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category="AI")
    virtual UAITask_MoveTo* MoveToLocation(FVector Position,float AcceptanceRadius = -1.f, EAIOptionFlag::Type StopOnOverlap = EAIOptionFlag::Default, EAIOptionFlag::Type AcceptPartialPath = EAIOptionFlag::Default,
        bool bUsePathfinding = true, bool bLockAILogic = true, bool bUseContinuosGoalTracking = false, EAIOptionFlag::Type ProjectGoalOnNavigation = EAIOptionFlag::Disable);

    UFUNCTION(BlueprintCallable, Category="AI")
    virtual UAITask_MoveTo* MoveToActor(AActor* ActorPtr,float AcceptanceRadius = -1.f, EAIOptionFlag::Type StopOnOverlap = EAIOptionFlag::Default, EAIOptionFlag::Type AcceptPartialPath = EAIOptionFlag::Default,
        bool bUsePathfinding = true, bool bLockAILogic = true, bool bUseContinuosGoalTracking = false, EAIOptionFlag::Type ProjectGoalOnNavigation = EAIOptionFlag::Disable);

	UPROPERTY()
	UAITask_MoveTo* MoveToTaskPtr;

	void MoveTo(FVector InGoalLocation, AActor* InGoalActor,
	float AcceptanceRadius, EAIOptionFlag::Type StopOnOverlap, EAIOptionFlag::Type AcceptPartialPath,
	bool bUsePathfinding, bool bLockAILogic, bool bUseContinuousGoalTracking, EAIOptionFlag::Type ProjectGoalOnNavigation);
};
