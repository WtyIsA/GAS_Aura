// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UnLuaInterface.h"
#include "GameFramework/GameModeBase.h"
#include "SelfGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MAIN_API ASelfGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:	
	UFUNCTION(BlueprintNativeEvent)
    FString OnPreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId);
	
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);

	UFUNCTION(BlueprintNativeEvent)
    FString OnLogin(APlayerController* PlayerController,const FString& Options);

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);
	
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;

	virtual void PostSeamlessTravel() override;

	UFUNCTION(BlueprintNativeEvent)
	void LuaPostSeamlessTravel();
	
	UFUNCTION(BlueprintNativeEvent)
	APlayerController* LuaSpawnPlayerController(ENetRole InRemoteRole, const FString& Options);

	virtual void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList) override;

	UFUNCTION(BlueprintNativeEvent)
	void LuaGetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList);
};