#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/PawnMovementComponent.h"
#include "AIPawnMovementComponent.generated.h"

UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent))
class MAIN_API UAIPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

public:

	DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMovementStatusChange, int, Status);

	UPROPERTY()
	FOnMovementStatusChange OnMovementStatusChange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AIPawnMovement)
	float MaxSpeed = 600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AIPawnMovement)
	bool bAllowPartialPath = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AIPawnMovement)
	float AcceptableRadius = 150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AIPawnMovement)
	bool bAllowStrafe = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AIPawnMovement)
	float RotateSpeed = 30;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AIPawnMovement)
	bool PrintDebugLog = false;

	UFUNCTION(BlueprintCallable)
	void MoveToLocation(FVector TargetPosition,int Timeout = 0,bool TeleportAfterTimeout = false);

	UFUNCTION(BlueprintCallable)
    void MoveToActor(const AActor* TargetActor,int Timeout = 0,bool TeleportAfterTimeout = false);

	UFUNCTION(BlueprintCallable)
    void LookAt(FVector TargetPosition);

	UFUNCTION(BlueprintCallable)
	void StopMoveNow();

protected:

	void RotateTo(FRotator TargetRotator);

	float CurrentTimeout = 0;

	float RequestStartTime = 0;
	
	bool bTeleportAfterTimeout = false;
	
	bool HasTarget = false;

	bool bChaseTarget = false;

	bool bIsLookAt = false;

	FVector LookAtPosition;

	FVector CurrentTargetPosition;

	TWeakObjectPtr<AActor> DestinationActor;

	void ResetParams();

	void SetInitParams(float Timeout,bool TeleportAfterTimeout);

	TWeakObjectPtr<AAIController> AIControllerPtr;

	TArray<FNavPathPoint> PathPoints;

	int CurrentPathIndex = 0;

	bool bStuck = false;

	bool bFinishRotate = false;

	void FindController();

	bool FindPath(bool UsePathFinding);
};