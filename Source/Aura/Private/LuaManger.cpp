// Fill out your copyright notice in the Description page of Project Settings.


#include "LuaManger.h"

#include "Cfg/CfgManager.h"
#include "UnLua/Private/UnLuaPrivate.h"

ULuaManger* ULuaManger::handle = nullptr;
UGameInstance* ULuaManger::m_pGame = nullptr;
ULuaManger* ULuaManger::Get(UGameInstance* pGameIns)
{
	if(handle == nullptr)   
	{
		m_pGame = pGameIns;
		handle = NewObject<ULuaManger>(pGameIns);
		handle->AddToRoot();
	}
	return  handle;
}

void ULuaManger::SetLuaSearchPathAndOcPath(const TArray<FString>& luaSearchPaths, const FString& ocPath)
{
	 FCfgManager::Get()->SetOcPath(ocPath);
	 UnLua::SetLuaSearchPaths(luaSearchPaths);
}

FString ULuaManger::GetLuaSrcPath(FString& luaScriptName) const
{
	luaScriptName = GLuaSrcRelativePath;
	return GLuaSrcFullPath;
}

void ULuaManger::PreInit()
{
	
}

void ULuaManger::Init_Implementation()
{
}

void ULuaManger::Start_Implementation()
{
}

void ULuaManger::Shutdown_Implementation()
{
}

void ULuaManger::UnInit()
{
	Shutdown();
// 	if(m_pGame)
// 	{
// 		if(_tHandle.IsValid())
// 			m_pGame->GetTimerManager().ClearTimer(_tHandle);
// 	}
// 	for(auto item : m_rootObjs)
// 	{
// 		if(IsValid(item))
// 			item->RemoveFromRoot();
// 	}
// 	m_rootObjs.Empty();
// 	LuaFree();
// 	FLevelCallback::Free();
// #ifdef USER_DB_SQLITE
// 	FLuaDBMgr::Free();
// #endif
//
// #if WITH_APPLICATION_CORE
// 	GLog->RemoveOutputDevice(FEvolutionOutputReportGs::GetAsFOutputDevice());
// #else
// 	GError = FPlatformOutputDevices::GetError();
// 	GWarn = FPlatformOutputDevices::GetFeedbackContext();
// #endif
// 	SetReportLogLevel(false,0);
// 	if(m_luaGcTick)
// 		delete m_luaGcTick;
// 	m_luaGcTick = nullptr;
// 	if (!IsRunningDedicatedServer())
// 	{
// 		FSlateApplication::Get().UnregisterInputPreProcessor(PreMouseButtonDown);
// 	}
}

void ULuaManger::Free()
{
	if(handle == nullptr)
		return;
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.RemoveAll(handle);
	FCoreDelegates::ApplicationWillEnterBackgroundDelegate.RemoveAll(handle);
	handle->UnInit();    
	handle->RemoveFromRoot();
	handle = nullptr;
	//CMessagePool::Free();

	// #if ENABLE_MEMORYTRACKER
	// 	FMemoryTracker::Free();
	// #endif
    
}