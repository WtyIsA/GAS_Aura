#include "LevelCallback.h"
#include "Engine/LevelStreaming.h"

#include "GameDelegates.h"
#include "LuaManger.h"


FLevelCallback* FLevelCallback::_handle = nullptr;

FLevelCallback::FLevelCallback()
{
}

FLevelCallback* FLevelCallback::Get()
{
	if(_handle == nullptr)
	{
		_handle = new FLevelCallback();
		_handle->RegisterCallback();
		/* 不需要
		GEngine->OnTravelFailure().AddLambda([](UWorld* InWorld, ETravelFailure::Type FailureType, const FString& ErrorString)
		{
			Get()->LevelLoadError(InWorld, FailureType, ErrorString);
		});
		*/
				
	}
	return _handle;
}

void FLevelCallback::Free()
{
	if(_handle)
	{
		FCoreUObjectDelegates::PreLoadMap.Remove(_handle->_preloadLevelDelegate);
		FWorldDelegates::OnPostWorldCleanup.Remove(_handle->_unloadLevelDelegate);
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(_handle->_loadLevelDelegate);
		GEngine->OnNetworkFailure().Remove(_handle->_DsFailedDelegate);
		FWorldDelegates::OnWorldBeginTearDown.Remove(_handle->_worldTornDown);
		FWorldDelegates::LevelAddedToWorld.Remove(_handle->_streamLevelLoad);
		FWorldDelegates::LevelRemovedFromWorld.Remove(_handle->_streamLevelUnLoad);
		delete _handle;
		_handle = nullptr;
	}
}

void FLevelCallback::RegisterCallback()
{
	_preloadLevelDelegate = FCoreUObjectDelegates::PreLoadMap.AddRaw(this,&FLevelCallback::OnPreLoadMap);
	_loadLevelDelegate = FCoreUObjectDelegates::PostLoadMapWithWorld.AddRaw(this, &FLevelCallback::PostLoadMapWithWorld);
	_unloadLevelDelegate = FWorldDelegates::OnPostWorldCleanup.AddRaw(this, &FLevelCallback::OnLevelUnloadComplete);
	_DsFailedDelegate = GEngine->OnNetworkFailure().AddLambda([](UWorld * World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
        {
            Get()->OnNetWorkFail(World, NetDriver, FailureType, ErrorString);
        });
	_worldTornDown = FWorldDelegates::OnWorldBeginTearDown.AddRaw(this, &FLevelCallback::OnWorldTornDown);
	_streamLevelLoad = FWorldDelegates::LevelAddedToWorld.AddRaw(this, &FLevelCallback::OnLoadStreamLevel);
	_streamLevelUnLoad = FWorldDelegates::LevelRemovedFromWorld.AddRaw(this, &FLevelCallback::OnUnLoadStreamLevel);
}

void FLevelCallback::OnPreLoadMap(const FString& levelName)
{

}


void FLevelCallback::PostLoadMapWithWorld(UWorld* World)
{
	if(World == nullptr)
	{
		//ULuaManger::Get()->OnLevelLoaded(nullptr,"");
		return;
	}

	ULuaManger::Get()->OnLevelLoaded(World,World->GetName());
}

void FLevelCallback::LevelLoadError(UWorld* InWorld, ETravelFailure::Type FailureType, const FString& ErrorString)
{
	//ULuaManger::Get()->OnLevelLoaded(nullptr,"");
}

void FLevelCallback::OnNetWorkFail(UWorld * World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{	
	if(World != nullptr)
	{
		ULuaManger::Get()->OnDsNetWorkFailure(World, World->GetName(), NetDriver, FailureType, ErrorString);
	}else
	{
		ULuaManger::Get()->OnDsNetWorkFailure(World, TEXT(""), NetDriver, FailureType, ErrorString);
	}
	
}

void FLevelCallback::OnLevelUnloadComplete(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{	
	if(World)
	{

		ULuaManger::Get()->OnLevelUnloadComplete(World, World->GetName(), bSessionEnded, bCleanupResources);
	}
}

void FLevelCallback::OnWorldTornDown(UWorld* pWorld)
{
	if(IsValid(pWorld))
	{
		for(auto item: pWorld->GetStreamingLevels())
		{
			OnUnLoadStreamLevel(item->GetLoadedLevel(), pWorld);;
		}
	}
}

void FLevelCallback::OnLoadStreamLevel(ULevel* pLevel, UWorld* pWrold)
{
	if(pLevel != nullptr)
	{
		UWorld* pOuter = Cast<UWorld>(pLevel->GetOuter());
		if(pOuter)
		{
            ULuaManger::Get()->OnLoadStreamLevel(pOuter->GetName(), pOuter);
		}
	}
}

void FLevelCallback::OnUnLoadStreamLevel(ULevel* pLevel, UWorld* pWrold)
{
	if(pLevel != nullptr)
	{
		UWorld* pOuter = Cast<UWorld>(pLevel->GetOuter());
		if(pOuter)
		{
            ULuaManger::Get()->OnUnLoadStreamLevel(pOuter->GetName(), pWrold);
		}
	}
}

void FLevelCallback::AddLevelNameToGPM(const FString& strLevelName)
{
	if(_GPMLevels.Contains(strLevelName))
		return;
	_GPMLevels.Add(strLevelName);
}

bool FLevelCallback::IsInGPMLevels(const FString& levelName)
{
	for(int i = 0; i < _GPMLevels.Num(); i++)
	{
		if(levelName.EndsWith(_GPMLevels[i]))
			return true;
	}
	return false;
}
