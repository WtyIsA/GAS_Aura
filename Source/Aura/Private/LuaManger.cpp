// Fill out your copyright notice in the Description page of Project Settings.


#include "LuaManger.h"

#include "LuaMessageTableOptInC.h"
#include "UnLuaEx.h"
#include "Aura/AuraLogChannels.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Cfg/CfgManager.h"
#include "Res/ResMgr.h"
#include "UnLua/Private/UnLuaPrivate.h"
#include "Engine/AssetManager.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Level/LevelCallback.h"
#include "LuaGcTickable.h"
#include "UnLuaEx.h"
ULuaManger* ULuaManger::handle = nullptr;
UGameInstance* ULuaManger::m_pGame = nullptr;

ULuaManger* ULuaManger::Get(UGameInstance* pGameIns)
{
	if (handle == nullptr)
	{
		m_pGame = pGameIns;
		handle = NewObject<ULuaManger>(pGameIns);
		handle->AddToRoot();
	}
	return handle;
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
	m_luaGcTick = nullptr;
	FLevelCallback::Get();
	// #if ENABLE_MEMORYTRACKER
	//     FMemoryTracker::Get()->Init();
	// #endif
	handle->_nLatentUuid = 1;
	handle->m_lastTickTime = FPlatformTime::Seconds();
	FCoreDelegates::OnBeginFrame.AddUObject(handle, &ULuaManger::Tick);

	// #ifdef USER_DB_SQLITE
	//     FLuaDBMgr::Get();
	// #endif
	//     if (handle->IsEmulator())
	//     {
	//         UE_LOG(LogAura, Display, TEXT("[main]IsEmulator set streaming.poolsize %d"), __LINE__);
	//         UKismetSystemLibrary::ExecuteConsoleCommand(m_pGame, TEXT("r.streaming.poolsize 4000"));
	//         UKismetSystemLibrary::ExecuteConsoleCommand(m_pGame, TEXT("r.AllowOcclusionQueries 0"));
	//     }
	//         
	//     if(IsRunningDedicatedServer())
	//     {
	//         m_luaGcTick = new FLuaGcTickable();
	//         FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this,&ULuaManger::OnPostLoadMapWithWorld);        
	//     }
	//     else
	//     {
	// #if !UE_SERVER
	//         FTextFormatter::Get().RegisterTextArgumentModifier(FTextFormatString::MakeReference(TEXT("fromconfig")),[](const FTextFormatString& InArgsString, const FTextFormatPatternDefinitionConstRef& InPatternDef) { return TextFormatArgumentModifier_FromConfig::Create(InArgsString); });
	//
	// #if WITH_APPLICATION_CORE
	//         GLog->AddOutputDevice(FEvolutionOutputReportGs::GetAsFOutputDevice());
	// #else
	//        GWarn = FEvolutionOutputWarn::GetAsFeedbackContext();
	//        GError = FEvolutionOutputError::GetAsOutputDeviceError();
	// #endif
	// #endif
	//         auto PreMouseDownCallback = [](const FPointerEvent&)
	//         {
	//             
	//         };
	//
	//         FMouseEventPreprocessor::FMouseDownEvent Event = FMouseEventPreprocessor::FMouseDownEvent::CreateUObject(this,&ULuaManger::OnPreMouseButtonDown);
	//         PreMouseButtonDown = MakeShared<FMouseEventPreprocessor>(Event);
	//         FSlateApplication::Get().RegisterInputPreProcessor(PreMouseButtonDown);
	//
	//         FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddLambda([this]()
	//         {
	//             OnAppEnteredForegroundDelegate.ExecuteIfBound(true);
	//         });
	//         FCoreDelegates::ApplicationWillEnterBackgroundDelegate.AddLambda([this]
	//         {
	//             OnAppEnteredForegroundDelegate.ExecuteIfBound(false);
	//         });
	//     }
}

void ULuaManger::SetTimer(float DeltaTime)
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
	// if(m_pGame)
	// {
	// 	if(_tHandle.IsValid())
	// 		m_pGame->GetTimerManager().ClearTimer(_tHandle);
	// }
	for (auto item : m_rootObjs)
	{
		if (IsValid(item))
			item->RemoveFromRoot();
	}
	m_rootObjs.Empty();
	LuaFree();
	FLevelCallback::Free();
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
	if (m_luaGcTick)
		delete m_luaGcTick;
	m_luaGcTick = nullptr;
	// 	if (!IsRunningDedicatedServer())
	// 	{
	// 		FSlateApplication::Get().UnregisterInputPreProcessor(PreMouseButtonDown);
	// 	}
}

void ULuaManger::Tick()
{
	double nowTick = FPlatformTime::Seconds();
	float ticktime = nowTick - m_lastTickTime;
	//CHttpDownloadMgr::Get()->Tick(ticktime);
	TickInLua(ticktime);
#ifdef USER_DB_SQLITE
	FLuaDBMgr::Get()->Tick(ticktime);
#endif
	m_lastTickTime = nowTick;
}

void ULuaManger::TickInLua_Implementation(float DeltaSeconds)
{
}

void ULuaManger::Free()
{
	if (handle == nullptr)
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

FString ULuaManger::GetPersistentDownloadDir() const
{
	//return CHttpDownloadMgr::Get()->m_savePath;
	return FString();
}

UObject* ULuaManger::GetResMgr()
{
	return UResMgr::Get();
}

UGameInstance* ULuaManger::GetGameInstance()
{
	return m_pGame;
}

double ULuaManger::GetCppTime()
{
	return FPlatformTime::Seconds();
}


void ULuaManger::AddUObjectToRoot(UObject* pObject)
{
	if (IsValid(pObject))
	{
		pObject->AddToRoot();
		m_rootObjs.Add(pObject);
	}
}

void ULuaManger::RemoveUObjectFromRoot(UObject* pObject)
{
	if (IsValid(pObject))
	{
		pObject->RemoveFromRoot();
		m_rootObjs.Remove(pObject);
	}
}


void ULuaManger::AddWidgetToGameViewPort(UUserWidget* widget, int nOrder)
{
	if (!IsValid(widget))
		return;
	if (m_pGame->GetGameViewportClient())
	{
		TSharedRef<SWidget> ViewportContent = widget->TakeWidget();
		m_pGame->GetGameViewportClient()->AddViewportWidgetContent(ViewportContent, nOrder);
	}
}

void ULuaManger::RemoveWidgetFromGameViewPort(UUserWidget* widget)
{
	if (!IsValid(widget))
		return;
	if (m_pGame->GetGameViewportClient())
	{
		TSharedRef<SWidget> ViewportContent = widget->TakeWidget();
		m_pGame->GetGameViewportClient()->RemoveViewportWidgetContent(ViewportContent);
	}
}


void ULuaManger::Log(int nLevel, FString& strModuleName, FString& strLogContent) const
{
	switch (nLevel)
	{
	case ELogVerbosity::Fatal:
		UE_LOG(LogAura, Fatal, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
		break;
	case ELogVerbosity::Error:
		UE_LOG(LogAura, Error, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
		break;
	case ELogVerbosity::Warning:
		UE_LOG(LogAura, Warning, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
		break;
	case ELogVerbosity::Display:
		UE_LOG(LogAura, Display, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
		break;
	case ELogVerbosity::Log:
		UE_LOG(LogAura, Log, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
		break;
	case ELogVerbosity::Verbose:
		UE_LOG(LogAura, Verbose, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
		break;
	case ELogVerbosity::VeryVerbose:
		UE_LOG(LogAura, VeryVerbose, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
		break;
	default:
		UE_LOG(LogAura, Display, TEXT("[%d][%s]%s"), GFrameNumber, *strModuleName, *strLogContent)
		break;
	}
}

UObject* ULuaManger::LoadRes(const FString& ResPath)
{
	UObject* pObj = StaticLoadObject(UObject::StaticClass(), nullptr, *ResPath);
	if (pObj)
		UnLua::AddObjectReferencer(pObj);
	return pObj;
}

void ULuaManger::LoadResAsync(const FString& ResPath, const FResLoadDelegate& Callback, int32 Priority, bool bCallbackAutoRelease)
{
	UAssetManager::GetStreamableManager().RequestAsyncLoad(ResPath, [ResPath, Callback,bCallbackAutoRelease]()
	{
		UObject* ResObject = StaticFindObject(UObject::StaticClass(), nullptr, *ResPath);
		if (ResObject == nullptr)
		{
			UE_LOG(LogAura, Warning, TEXT("[LoadResAsync]load res %s failed"), *ResPath);
		}
		UnLua::AddObjectReferencer(ResObject);
		Callback.ExecuteIfBound(ResPath, ResObject);
		if (bCallbackAutoRelease)
			UnLua::ReleaseLuaCallback(Callback.GetUObject(), Callback.GetFunctionName().ToString());
	}, Priority);
}

void ULuaManger::LoadMultiResAsync(TArray<FString>& ResPathList, const FMutliResLoadDelegate& Callback,
                                   int32 Priority, bool bCallbackAutoRelease)
{
	if (ResPathList.Num() > 0)
	{
		TArray<FSoftObjectPath> SoftPathList;
		for (FSoftObjectPath path : ResPathList)
		{
			SoftPathList.Add(path);
		}

		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(SoftPathList, FStreamableDelegate::CreateLambda(
			                                                             [ResPathList, SoftPathList, Callback,bCallbackAutoRelease]()
			                                                             {
				                                                             TArray<UObject*> loadedObjects;
				                                                             for (FSoftObjectPath path : SoftPathList)
				                                                             {
					                                                             loadedObjects.Add(path.ResolveObject());
					                                                             UnLua::AddObjectReferencer(path.ResolveObject());
				                                                             }

				                                                             Callback.Execute(ResPathList, loadedObjects);
				                                                             if (bCallbackAutoRelease)
					                                                             UnLua::ReleaseLuaCallback(Callback.GetUObject(), Callback.GetFunctionName().ToString());
			                                                             }), Priority);
	}
	else
	{
		UE_LOG(LogAura, Warning, TEXT("[LoadMultiResAsync]ResPathList is empty"));
		Callback.Execute(ResPathList, TArray<UObject*>());
		if (bCallbackAutoRelease)
			UnLua::ReleaseLuaCallback(Callback.GetUObject(), Callback.GetFunctionName().ToString());
	}
}

UObject* ULuaManger::CreateObject(const UClass* Class)
{
	check(m_pGame);

	if (!Class)
	{
		UE_LOG(LogAura, Error, TEXT("[CreateObject]Class is null."));
		return nullptr;
	}

	return NewObject<UObject>(m_pGame, Class);
}

UUserWidget* ULuaManger::CreateUserWidget(TSubclassOf<UUserWidget> WidgetType)
{
	check(m_pGame);
	return UWidgetBlueprintLibrary::Create(m_pGame, WidgetType, nullptr);
}

UUserWidget* ULuaManger::CreateUserWidgetFromPath(const FString& ResPath, bool bAsync, const FResLoadDelegate& Callback, int32 Priority, bool bCallbackAutoRelease)
{
	if (ResPath.Len() == 0)
	{
		UE_LOG(LogAura, Warning, TEXT("[CreateUserWidgetFromPath]ResPathList is empty"));
		if (bAsync)
		{
			Callback.ExecuteIfBound(ResPath, nullptr);
			if (bCallbackAutoRelease)
				UnLua::ReleaseLuaCallback(Callback.GetUObject(), Callback.GetFunctionName().ToString());
		}
		return nullptr;
	}
	if (!bAsync)
	{
		UClass* pResClass = Cast<UClass>(StaticLoadObject(UObject::StaticClass(), nullptr, *ResPath));
		if (!pResClass)
			return nullptr;
		return UWidgetBlueprintLibrary::Create(m_pGame, pResClass, nullptr);
	}
	UAssetManager::GetStreamableManager().RequestAsyncLoad(ResPath, [ResPath, Callback,bCallbackAutoRelease]()
	{
		UClass* pResClass = Cast<UClass>(StaticFindObject(UObject::StaticClass(), nullptr, *ResPath));
		if (!pResClass)
		{
			UE_LOG(LogAura, Warning, TEXT("[LoadResAsync]load res %s failed"), *ResPath);
		}
		UUserWidget* WidgetObj = UWidgetBlueprintLibrary::Create(m_pGame, pResClass, nullptr);
		Callback.ExecuteIfBound(ResPath, WidgetObj);
		if (bCallbackAutoRelease)
			UnLua::ReleaseLuaCallback(Callback.GetUObject(), Callback.GetFunctionName().ToString());
	}, Priority);
	return nullptr;
}


void ULuaManger::OnDownloadComplete_Implementation(const FString& strUel, bool bSuccess, const FString& contentOrsavePath, bool bSaveFile, int errorCode)
{
}

FString ULuaManger::GetIniFilePath(FString& Name)
{
	if (Name == TEXT("Game"))
	{
		return GGameIni;
	}
	else if (Name == TEXT("Input"))
	{
		return GInputIni;
	}
	else if (Name == TEXT("GameUserSettings"))
	{
		return GGameUserSettingsIni;
	}
	else if (Name == TEXT("Scalability"))
	{
		return GScalabilityIni;
	}
	else if (Name == TEXT("Hardware"))
	{
		return GHardwareIni;
	}
	return GEngineIni;
}


void ULuaManger::OnLoadStreamLevel_Implementation(const FString& worldName, UWorld* pWorld)
{
}

void ULuaManger::OnUnLoadStreamLevel_Implementation(const FString& worldName, UWorld* pWorld)
{
}

void ULuaManger::OnLevelLoaded_Implementation(UWorld* pWorld, const FString& levelName)
{
}

void ULuaManger::OnDsNetWorkFailure_Implementation(UWorld* World, const FString& worldName, UNetDriver* NetDriver, int32 nailureType, const FString& ErrorString)
{
}

void ULuaManger::OnLevelUnloadComplete_Implementation(UWorld* World, const FString& worldName, bool bSessionEnded,
                                                      bool bCleanupResources)
{
}


static int32 ULuaManger_UnPack(lua_State* L)
{
	if (lua_type(L, 1) != LUA_TTABLE)
	{
		return 0;
	}
	Table* pTable = (Table*)lua_topointer(L, 1);
	if (pTable->alimit == 0)
		return 0;
	unsigned int nSize = 0;
	for (unsigned int i = pTable->alimit; i > 0; i--)
	{
		if (!ttisnil(pTable->array + i - 1))
		{
			nSize = i;
			break;
		}
	}
	for (unsigned int i = 0; i < nSize; i++)
	{
		CLuaMessageTableOptInC::PushValue(pTable->array + i, L);
	}
	return nSize;
}

static const luaL_Reg ULuaMangerLib[] =
{
	{"UnPack", ULuaManger_UnPack},
	// 	{"SetTextBlockFormat", SetTextBlockFormat},
	// 	{"ReportTable", ULuaManger_Report},
	// 	{"PrintLuaMem", ULuaManger_LuaGcMem},
	// 	{"PrintLiveObjects", ULuaManger_PrintLiveObjects},
	// 	{"CheckLuaNeedFullGc", ULuaManger_LuaFullGc},
	// 	{"PrintObjectByClassName",ULuaManger_PrintObjectByClassName},
	// #if ENABLE_MEMORYTRACKER
	// 	{"SetMemoryTracker",ULuaManger_SetMemoryTracker},
	// 	{"AddMemoryTrackerTag",ULuaManger_AddMemoryTrackerTag},
	// 	{"RemoveMemoryTrackerTag",ULuaManger_RemoveMemoryTrackerTag},
	// 	{"SaveMemoryTracker",ULuaManger_SaveMemoryTracker},
	// 	{"ClearMemoryTracker",ULuaManger_ClearMemoryTracker},
	// #endif    
	// 	{"ReleaseDelegate",ULuaManger_ReleaseDelegate },
	// 	{"JsonToLuaTable",ULuaManger_JsonToLuaTable},
	// 	{"LuaTableToJson",ULuaManger_LuaTableToJson},
	// 	{"TextFormat",TextFormat},
	{nullptr, nullptr}
};

BEGIN_EXPORT_REFLECTED_CLASS(ULuaManger)
	ADD_LIB(ULuaMangerLib)
END_EXPORT_CLASS()

IMPLEMENT_EXPORTED_CLASS(ULuaManger)
