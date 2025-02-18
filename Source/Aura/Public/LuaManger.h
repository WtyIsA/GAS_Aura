// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "UObject/NoExportTypes.h"
#include "LuaManger.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FResLoadDelegate, const FString&, ResPath, UObject*, ResObject);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FMutliResLoadDelegate, TArray<FString>, ResPathList, TArray<UObject*>, ResList);

DECLARE_DYNAMIC_DELEGATE_FiveParams(FOnHttpDownloadComplete, const FString&, strUrl, bool, bSuccess, const FString&, contentOrsavePath, bool, bSaveFile, int, errorCode);

DECLARE_DYNAMIC_DELEGATE_OneParam(FSdkBoolDelegate, bool, bState);

DECLARE_DYNAMIC_DELEGATE_FiveParams(FSdkBoolIntThreeStringsDelegate, bool, bSuccess, int, errorCode, const FString&, strDta1, const FString&, strData2, const FString&, strData3);

DECLARE_DYNAMIC_DELEGATE_FourParams(FSdkBoolTwoIntStringDelegate, bool, bSuccess, int, param1, int, param2, const FString&, strData);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FSdkBoolLongDelegate, bool, bSuccess, uint64, userId);
class FLuaGcTickable;
/**
 * 
 */
UCLASS()
class AURA_API ULuaManger : public UObject, public IUnLuaInterface
{
	GENERATED_BODY()
	void UnInit();

public:
	static ULuaManger* Get(UGameInstance* pGameIns = NULL);
	static void Free();

	virtual FString GetModuleName_Implementation() const
	{
		if (IsRunningDedicatedServer())
		{
			return TEXT("Server.LuaMgr");
		}
		else
		{
			return TEXT("Client.LuaMgr");
		}
	}

	void Tick();
	UFUNCTION()
	void SetTimer(float DeltaTime);
	
	UFUNCTION()
	void SetLuaSearchPathAndOcPath(const TArray<FString>& luaSearchPaths, const FString& ocPath);

	UFUNCTION(BlueprintNativeEvent)
	void TickInLua(float DeltaSeconds);
	
	UFUNCTION()
	FString GetLuaSrcPath(FString& luaScriptName) const;

	void PreInit();
	UFUNCTION(BlueprintNativeEvent)

	void Init();

	UFUNCTION(BlueprintNativeEvent)
	void Start();

	UFUNCTION(BlueprintNativeEvent)
	void Shutdown();

	static ULuaManger* handle;

	UFUNCTION()
	FString GetPersistentDownloadDir() const;

	UFUNCTION()
	UObject* GetResMgr();

	UFUNCTION(BlueprintImplementableEvent)
	void LuaFree();

	UFUNCTION()
	void AddUObjectToRoot(UObject* pObject);

	UFUNCTION()
	void RemoveUObjectFromRoot(UObject* pObject);

#pragma region UUserWidget
    
    UFUNCTION()
    void AddWidgetToGameViewPort(UUserWidget* widget, int nOrder);
    UFUNCTION()
    void RemoveWidgetFromGameViewPort(UUserWidget* widget);

#pragma endregion
	
public:
	static UGameInstance* m_pGame;
	TArray<UObject*>   m_rootObjs;   
	UFUNCTION()
	UGameInstance* GetGameInstance();

	UFUNCTION()
	UObject* LoadRes(const FString& ResPath);

	UFUNCTION()
	void LoadResAsync(const FString& ResPath, const FResLoadDelegate& Callback = FResLoadDelegate(), int32 Priority = 0, bool bCallbackAutoRelease=false);
        
	UFUNCTION()
	void LoadMultiResAsync(TArray<FString>& ResPathList, const FMutliResLoadDelegate& Callback = FMutliResLoadDelegate(), int32 Priority = 0, bool bCallbackAutoRelease=false);

	UFUNCTION()
	UObject* CreateObject(const UClass* Class);

	UFUNCTION()
	UUserWidget* CreateUserWidget(TSubclassOf<class UUserWidget> WidgetType);
	UFUNCTION()
	UUserWidget* CreateUserWidgetFromPath(const FString& ResPath, bool bAsync=true, const FResLoadDelegate& Callback = FResLoadDelegate(), int32 Priority = 0, bool bCallbackAutoRelease=false);
	
	UFUNCTION()
	void Log(int nLevel, FString& strModuleName, FString& strLogContent)const;

	UFUNCTION()
	static double GetCppTime();
#pragma region level

	UFUNCTION(BlueprintNativeEvent)
	void OnLoadStreamLevel(  const FString& worldName, UWorld* pWorld);
	UFUNCTION(BlueprintNativeEvent)
	void OnUnLoadStreamLevel(  const FString& worldName, UWorld* pWorld);
	UFUNCTION(BlueprintNativeEvent)
	void OnLevelLoaded(UWorld* pWorld,const FString& levelName);
	UFUNCTION(BlueprintNativeEvent)
	void OnDsNetWorkFailure(UWorld * World, const FString& worldName, UNetDriver *NetDriver, int32 nailureType, const FString& ErrorString);
	UFUNCTION(BlueprintNativeEvent)
	void OnLevelUnloadComplete(UWorld * World, const FString& worldName, bool bSessionEnded, bool bCleanupResources);

#pragma endregion

	
#pragma region http
	UFUNCTION(BlueprintNativeEvent)
	void OnDownloadComplete(const FString& strUrl, bool bSuccess, const FString& contentOrsavePath, bool bSaveFile, int errorCode = 0);
#pragma endregion

#pragma region sdk

	FSdkBoolDelegate OnSdkInitFinishCallBack;
	FSdkBoolIntThreeStringsDelegate OnGetCountryCodeCallback;
	FSdkBoolTwoIntStringDelegate OnGetFriendChainInfo;
	FSdkBoolLongDelegate OnGetFriendChainPermission;
#pragma endregion

private:
	FString GetIniFilePath(FString& filePath);
	int32   _nLatentUuid;
	FTimerHandle _tHandle;
	double          m_lastTickTime;
	FLuaGcTickable* m_luaGcTick;

};
