// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolingManager.generated.h"
DECLARE_DYNAMIC_DELEGATE_OneParam(FActorParamFunctionDelegate, AActor*, Actor);

USTRUCT(BlueprintType)
struct FPoolingManagerData
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere, Category = "Pooling Manager")
	int32 MaxCount;

	UPROPERTY(EditAnywhere, Category = "Pooling Manager")
	int32 InitCount;

	UPROPERTY()
	TSet<AActor*> ReadyActors;

	UPROPERTY()
	TSet<AActor*> WaitRecycleActors;
	FPoolingManagerData()
		: ActorClass(nullptr)
		  , MaxCount(99999999)
		  , InitCount(0)
	{
	}

	bool operator !=(const FPoolingManagerData& InActorData) const
	{
		return ActorClass != InActorData.ActorClass;
	}

	void WaitRecycleToReady();

	void ClearAll();
};

UCLASS()
class MAIN_API APoolingManager : public AActor
{
	GENERATED_BODY()

public:
	APoolingManager();

	TMap<TSubclassOf<AActor>, FPoolingManagerData*> PoolingData;

	UPROPERTY()
	TMap<TSubclassOf<AActor>, FActorParamFunctionDelegate> InitFuntionsMap;

	UPROPERTY(EditAnywhere, Category="Pooling Manger")
	FVector InitPosition;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual bool CanBeClusterRoot() const override;

private:
	bool bServer;

	void InitPoolActor(AActor* PoolActor) const;

	FPoolingManagerData* GetPoolingManagerData(const TSubclassOf<AActor> ActorClass);

	void SetMinGCClusterSize(int32 Size);

	void OnLevelActorDeleted(AActor* InActor);

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	AActor* GetActor(TSubclassOf<AActor> ActorClass,const FActorParamFunctionDelegate& InitFunction);

	UFUNCTION(BlueprintCallable)
	void SetInitFunction(TSubclassOf<AActor> ActorClass, const FActorParamFunctionDelegate& Function);

	UFUNCTION(BlueprintCallable)
	void RecycleActor(AActor* Actor,const FActorParamFunctionDelegate& RecycleActorFunction);

	UFUNCTION()
	void ClearAll();
};

