// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EStateType.h"
#include "FSMState.h"
#include "Components/ActorComponent.h"
#include "FSMSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class  UFSMSystem : public UActorComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UFSMSystem();
	bool EnableLog = true;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	TMap<EStateType, class UFSMState*> StateMap;
	UPROPERTY()
	TArray<UFSMState*> Stack;
	UPROPERTY()
	UFSMState* CurState;
	UPROPERTY()
	UAnimInstance* CurAnimInstance;
	void Print(FString s);
	
	UFUNCTION(BlueprintCallable)
	void Start(UAnimInstance* animInstance,EStateType type);
	UFUNCTION(BlueprintCallable)
	void AddState(EStateType type, const TSubclassOf<UFSMState> StateClass);
	void ChangeState(EStateType type, const TArray<void*>* args, EStateType nextType = EStateType::Default);
	UFUNCTION(BlueprintCallable)
	void ChangeState(EStateType type, EStateType nextType = EStateType::Default);
	UFUNCTION(BlueprintCallable,Server,Reliable)
	void ServerChangeState(EStateType type,EStateType nextType = EStateType::Default);
	UFUNCTION(NetMulticast,Reliable)
	void MultiCastChangeState(EStateType type,EStateType nextType = EStateType::Default);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	FString GetTypeString(EStateType type);
};

inline FString UFSMSystem::GetTypeString(EStateType type)
{
	return "Type:"+StaticEnum<EStateType>()->GetNameStringByValue(static_cast<uint8>(type));
}
