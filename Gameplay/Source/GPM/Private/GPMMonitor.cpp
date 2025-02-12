// Fill out your copyright notice in the Description page of Project Settings.


#include "GPMMonitor.h"
#include "GPMCXXBridge.h"
using namespace GMSDK;

string UGPMMonitor::GameType = "game_scene";

UGPMMonitor::UGPMMonitor()
{
	CurrentScene = "";
	bIsRunning = false;
}

UGPMMonitor::~UGPMMonitor()
{

}

void UGPMMonitor::Tick(float DeltaTime)
{
	if (bIsRunning)
	{
		GPMCXXBridge::LogFrameEnd();
	}
}

TStatId UGPMMonitor::GetStatId() const
{
	return UObject::GetStatID();
}

void UGPMMonitor::LogSceneStart(const string& SceneName)
{
	if (bIsRunning)
	{
		GPMCXXBridge::LogSceneEnd(GameType, CurrentScene);
	}
	GPMCXXBridge::LogSceneStart(GameType, SceneName);
	CurrentScene = SceneName;
	bIsRunning = true;
	LogSceneInfo("rom_size_avail");
}

void UGPMMonitor::LogSceneLoaded()
{
	if (bIsRunning)
	{
		GPMCXXBridge::LogSceneLoaded(GameType, CurrentScene);
	}
}

void UGPMMonitor::LogSceneEnd()
{
	if (bIsRunning)
	{
		bIsRunning = false;
		GPMCXXBridge::LogSceneEnd(GameType, CurrentScene);
		CurrentScene = "";
	}
}

void UGPMMonitor::LogGlobalInfo(const string& Key, const string& Value)
{
	GPMCXXBridge::LogGlobalInfo(GameType, Key, Value);
}

void UGPMMonitor::LogGlobalInfo(const string& Key, const int Value)
{
	GPMCXXBridge::LogGlobalInfo(GameType, Key, Value);
}

void UGPMMonitor::LogGlobalInfo(const string& Key)
{
	GPMCXXBridge::LogGlobalInfo(GameType, Key);
}

void UGPMMonitor::LogSceneInfo(const string& Key, const string& Value)
{
	if (bIsRunning)
	{
		GPMCXXBridge::LogSceneInfo(GameType, CurrentScene, Key, Value);
	}
}

void UGPMMonitor::LogSceneInfo(const string& Key, const int Value)
{
	if (bIsRunning)
	{
		GPMCXXBridge::LogSceneInfo(GameType, CurrentScene, Key, Value);
	}
}

void UGPMMonitor::LogSceneInfo(const string& Key)
{
	if (bIsRunning)
	{
		GPMCXXBridge::LogSceneInfo(GameType, CurrentScene, Key);
	}
}
