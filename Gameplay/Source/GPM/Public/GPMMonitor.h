// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <string>
#include "Tickable.h"
#include "GPMMonitor.generated.h"

using std::string;
/**
 * 
 */
UCLASS()
class GPM_API UGPMMonitor : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
public:
	UGPMMonitor();
	~UGPMMonitor();

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	void LogSceneStart(const string& SceneName);
	void LogSceneLoaded();
	void LogSceneEnd();
	static void LogGlobalInfo(const string& Key, const string& Value);
	static void LogGlobalInfo(const string& Key, const int Value);
	static void LogGlobalInfo(const string& Key);
	void LogSceneInfo(const string& Key, const string& Value);
	void LogSceneInfo(const string& Key, const int Value);
	void LogSceneInfo(const string& Key);
private:
	bool bIsRunning;
	static string GameType;
	string CurrentScene ;
};
