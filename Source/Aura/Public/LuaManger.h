// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "UObject/NoExportTypes.h"
#include "LuaManger.generated.h"

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
	FString GetLuaSrcPath(FString& luaScriptName)const;
	
	void PreInit();
	UFUNCTION(BlueprintNativeEvent)
	
	void Init();
	
	UFUNCTION(BlueprintNativeEvent)
	void Start();

	UFUNCTION(BlueprintNativeEvent)
	void Shutdown();

	static ULuaManger* handle;

public:
	static UGameInstance* m_pGame;
};
