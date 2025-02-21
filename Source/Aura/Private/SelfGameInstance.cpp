// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfGameInstance.h"

#include "LuaManger.h"
#include "UtilsProject.h"
#include "Res/ResMgr.h"
#include "UnLua/Private/UnLuaPrivate.h"

USelfGameInstance::USelfGameInstance() : UGameInstance()
{
}

void USelfGameInstance::Init()
{
	Super::Init();
	// #if !PLATFORM_WINDOWS && !PLATFORM_MAC
	// 	FUtilsProject::MountSavePaks();
	// #endif
	UnLua::Startup();
	ULuaManger::Get(this)->PreInit();
	ULuaManger::Get()->Init();
}

void USelfGameInstance::OnStart()
{
	Super::OnStart();

	// 	UShortcutkeyMgr::Get();
	//
	ULuaManger::Get()->Start();
	// #if USER_AKAUDIO 
	// 	if (!IsRunningDedicatedServer() && !IsRunningCommandlet())
	// 	{
	// 		FSlateApplication& CurrentSlateApp = FSlateApplication::Get();
	// 		CurrentSlateApp.InitializeSound(TSharedRef<FSoundDevice>(new FSoundDevice()));
	// 	}
	// #endif
}

void USelfGameInstance::Shutdown()
{
	// UE_LOG(GAME, Display, TEXT("UEvolutionGameInstance::Shutdown"));
	//    
	UResMgr::Free();
	// UShortcutkeyMgr::Free();
	// CHttpDownloadMgr::Free();
	//  
	ULuaManger::Get(this)->Shutdown();
	ULuaManger::Free();
	// UFPSManager::Free();
	UnLua::Shutdown();
	GConfig->Flush(true, FUtilsProject::GetUserConfigPath());
	Super::Shutdown();
}

void USelfGameInstance::LoadSubLevels()
{
	// #if WITH_EDITOR
	// 	UE_LOG(GAME, Display, TEXT("UEvolutionGameInstance::LoadSubLevels"));
	// 	UWorld* World = GetWorld();
	// 	UWorldComposition* WorldComposition = World->WorldComposition;
	// 	if(WorldComposition)
	// 	{
	// 		WorldComposition->Rescan();
	// 		World->SetStreamingLevels(WorldComposition->TilesStreaming);
	// 	}
	// #endif
}
