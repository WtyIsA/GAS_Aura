// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "GameFramework/Actor.h"
#include "ActorRandom.generated.h"

UCLASS()
class MAIN_API AActorRandom : public AActor, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AActorRandom();

	virtual void PostLoad() override;
	
	FString GetModuleName_Implementation() const override;

#pragma region Blueprint properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorRandom", meta=(ToolTip="Random Group Id(cfgId)."))
	int CfgId;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorRandom", meta=(ToolTip="Actors in this container will only spawn one of them."))
	TArray<AActor*> RandomActors;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ActorRandom", meta=(ToolTip="Actors in this container will always spawn."))
	TSet<AActor*> PerpetualActors;
#pragma endregion

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void NotifyDestroyed();
#if WITH_EDITOR
	virtual void NotifyHiddenEdChange(bool bCurHidden);
#endif

	void CheckIds();

	void OnCheckIds(TArray<int> CfgIds);
	int GetCfgIdFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable)
	virtual void DestroyChild(AActor* Child);

	UFUNCTION(BlueprintCallable)
	void DestroySelfAndAllChildren();

	UFUNCTION(BlueprintCallable)
	void DestroySelf();
#if WITH_EDITOR
	virtual void EdHideChildChange(AActor* Child, bool bCurHidden);
#endif

	UFUNCTION(BlueprintCallable)
	virtual void Random(bool bFromClick = false);

	bool IsInRange(const int& InResult, const TPair<AActor*,int>& InPair);
	bool IsStartPIE() const;
public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	virtual void PostActorCreated() override;
#endif

	UFUNCTION(BlueprintImplementableEvent)
	int GetActorRandomProperty(AActor* InTargetActor);

	UPROPERTY(EditAnywhere)
	FString ConfigPath;
};
