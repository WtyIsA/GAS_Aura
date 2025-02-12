// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfGameMode.h"
#include "LuaManger.h"
#include "SelfPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UnLuaEx.h"
FString ASelfGameMode::OnPreLogin_Implementation(const FString& Options, const FString& Address,
    const FUniqueNetIdRepl& UniqueId)
{
    return "";
}

void ASelfGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
                             FString& ErrorMessage)
{
    Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
    if (!ErrorMessage.IsEmpty())
    {
        UE_LOG(GAME, Error, TEXT("[ASelfGameMode]PreLogin Error:ErrorMessage=%s,Options=%s,Address=%s"),
            *ErrorMessage, *Options, *Address);
        return;
    }
    ErrorMessage = OnPreLogin(Options, Address, UniqueId);
}

APlayerController* ASelfGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
    const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
    APlayerController* PlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
    if(PlayerController)
    {
        ErrorMessage = OnLogin(PlayerController,Options);
        if (!ErrorMessage.IsEmpty())
        {
            UE_LOG(GAME, Error, TEXT("[ASelfGameMode]Login Error:ErrorMessage=%s,Options=%s"),
                *ErrorMessage, *Options);
            PlayerController->Destroy();
            PlayerController = NULL;
        }
    }
    return PlayerController;
}

APlayerController* ASelfGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
    return LuaSpawnPlayerController(InRemoteRole,Options);
}

void ASelfGameMode::PostSeamlessTravel()
{
    Super::PostSeamlessTravel();
    LuaPostSeamlessTravel();
}

void ASelfGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
    Super::GetSeamlessTravelActorList(bToTransition, ActorList);
    LuaGetSeamlessTravelActorList(bToTransition,ActorList);
}

void ASelfGameMode::LuaGetSeamlessTravelActorList_Implementation(bool bToTransition, TArray<AActor*>& ActorList)
{
}

void ASelfGameMode::LuaPostSeamlessTravel_Implementation()
{
    
}

APlayerController* ASelfGameMode::LuaSpawnPlayerController_Implementation(ENetRole InRemoteRole, const FString& Options)
{
    return  nullptr;
}

FString ASelfGameMode::OnLogin_Implementation(APlayerController* PlayerController, const FString& Options)
{
    return "";
}
