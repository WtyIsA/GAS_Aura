#pragma once

class FLevelCallback
{
private:
	FLevelCallback();
	static FLevelCallback* _handle;
	void RegisterCallback();
	void OnNetWorkFail(UWorld * World, UNetDriver *NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	FDelegateHandle		_unloadLevelDelegate;
	FDelegateHandle		_loadLevelDelegate;
	FDelegateHandle		_preloadLevelDelegate;
	FDelegateHandle		_DsFailedDelegate;
	FDelegateHandle		_streamLevelLoad;
	FDelegateHandle		_streamLevelUnLoad;
	FDelegateHandle		_worldTornDown;
	TArray<FString>		_steamlevels;
	void OnWorldTornDown(UWorld* pWorld);   
    void OnLoadStreamLevel( ULevel* pLevel, UWorld* pWrold);
	void OnUnLoadStreamLevel( ULevel* pLevel, UWorld* pWrold);
	void OnPreLoadMap(const FString& levelName);
	bool IsInGPMLevels(const FString& levelName);
	TArray<FString>		_GPMLevels;
public:
	static FLevelCallback*  Get();
	static void Free();
	void PostLoadMapWithWorld(UWorld *World);
	void LevelLoadError(UWorld* InWorld, ETravelFailure::Type FailureType, const FString& ErrorString);
	void OnLevelUnloadComplete(UWorld* World, bool bSessionEnded, bool bCleanupResources);
	void AddLevelNameToGPM(const FString& strLevelName);
	
};
