// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfGameInstance.h"

#include "LuaManger.h"
#include "UtilsProject.h"
#include "UnLua/Private/UnLuaPrivate.h"
#include "Res/ResMgr.h"
#include "Http/HttpDownloadMgr.h"
#include "Input/ShortcutkeyMgr.h"
#include "Engine/WorldComposition.h"
#include "Gameplay/FPSManager.h"
#include "UI/SlateSound/SoundDevice.h"

USelfGameInstance::USelfGameInstance() : UGameInstance()
{    
}

void USelfGameInstance::Init()
{
    Super::Init();
#if !PLATFORM_WINDOWS && !PLATFORM_MAC
	FUtilsProject::MountSavePaks();
#endif
    UnLua::Startup();
    ULuaManger::Get(this)->PreInit();    
    ULuaManger::Get()->Init();
    ULuaManger::Get()->PostInit();
}

void USelfGameInstance::OnStart()
{
    Super::OnStart();
    
    UShortcutkeyMgr::Get();

    ULuaManger::Get()->Start();
#if USER_AKAUDIO 
    if (!IsRunningDedicatedServer() && !IsRunningCommandlet())
    {
        FSlateApplication& CurrentSlateApp = FSlateApplication::Get();
        CurrentSlateApp.InitializeSound(TSharedRef<FSoundDevice>(new FSoundDevice()));
    }
#endif
}

void USelfGameInstance::Shutdown()
{
    UE_LOG(GAME, Display, TEXT("UEvolutionGameInstance::Shutdown"));
    
    UResMgr::Free();
    UShortcutkeyMgr::Free();
    CHttpDownloadMgr::Free();
  
    //ULuaManger::Get()->Shutdown();
    ULuaManger::Free();
    UFPSManager::Free();
    UnLua::Shutdown();
    GConfig->Flush(true,FUtilsProject::GetUserConfigPath());
    Super::Shutdown();
}

void USelfGameInstance::LoadComplete(const float LoadTime, const FString& MapName)
{
    Super::LoadComplete(LoadTime, MapName);
    ULuaManger::Get()->OnPersistantLevelLoaded();
}

void USelfGameInstance::LoadSubLevels()
{
#if WITH_EDITOR
    UE_LOG(GAME, Display, TEXT("USelfGameInstance::LoadSubLevels"));
    UWorld* World = GetWorld();
    UWorldComposition* WorldComposition = World->WorldComposition;
    if(WorldComposition)
    {
        WorldComposition->Rescan();
        World->SetStreamingLevels(WorldComposition->TilesStreaming);
    }
#endif
}
