// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "UObject/NoExportTypes.h"
#include "LuaManger.generated.h"

DECLARE_DYNAMIC_DELEGATE_FiveParams(FOnHttpDownloadComplete, const FString&, strUrl, bool, bSuccess, const FString&, contentOrsavePath, bool, bSaveFile, int, errorCode);

DECLARE_DYNAMIC_DELEGATE_OneParam(FSdkBoolDelegate, bool, bState);

DECLARE_DYNAMIC_DELEGATE_FiveParams(FSdkBoolIntThreeStringsDelegate, bool, bSuccess, int, errorCode, const FString&, strDta1, const FString&, strData2, const FString&, strData3);

DECLARE_DYNAMIC_DELEGATE_FourParams(FSdkBoolTwoIntStringDelegate, bool, bSuccess, int, param1, int, param2, const FString&, strData);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FSdkBoolLongDelegate, bool, bSuccess, uint64, userId);

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

	UFUNCTION()
	void SetLuaSearchPathAndOcPath(const TArray<FString>& luaSearchPaths, const FString& ocPath);

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

public:
	static UGameInstance* m_pGame;

	UFUNCTION()
	UGameInstance* GetGameInstance();

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
};
