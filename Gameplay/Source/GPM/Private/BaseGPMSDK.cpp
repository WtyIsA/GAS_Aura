// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGPMSDK.h"
#include "GPMGlobalInfo.h"
#include "GPMGraphicLevel.h"

namespace GMSDK 
{
	BaseGPMSDK::BaseGPMSDK()
	{
		GPMGlobalInfo::Init();
		GPMGraphicLevel::RequestGraphicLevel();
		if (!Monitor.IsValid())
		{
			Monitor = NewObject<UGPMMonitor>();
			Monitor->AddToRoot();
		}
	}

	BaseGPMSDK::~BaseGPMSDK()
	{
		if (Monitor != nullptr)
		{
			Monitor->RemoveFromRoot();
			Monitor = nullptr;
		}
	}

	int BaseGPMSDK::GraphicLevel()
	{
		return GPMGraphicLevel::RequestGraphicLevel();
	}

	void BaseGPMSDK::LogSceneStart(const string& SceneName)
	{
		if (Monitor.IsValid())
		{
			Monitor->LogSceneStart(SceneName);
		}
	}

	void BaseGPMSDK::LogSceneLoaded()
	{
		if (Monitor.IsValid())
		{
			Monitor->LogSceneLoaded();
		}
	}

	void BaseGPMSDK::LogSceneEnd()
	{
		if (Monitor.IsValid())
		{
			Monitor->LogSceneEnd();
		}
	}

	void BaseGPMSDK::LogGlobalInfo(const string& Key, const string& Value)
	{
		UGPMMonitor::LogGlobalInfo(Key, Value);
	}

	void BaseGPMSDK::LogGlobalInfo(const string& Key, const int Value)
	{
		UGPMMonitor::LogGlobalInfo(Key, Value);
	}

	void BaseGPMSDK::LogGlobalInfo(const string& Key)
	{
		UGPMMonitor::LogGlobalInfo(Key);
	}

	void BaseGPMSDK::LogSceneInfo(const string& Key, const string& Value)
	{
		if (Monitor.IsValid())
		{
			Monitor->LogSceneInfo(Key, Value);
		}
	}

	void BaseGPMSDK::LogSceneInfo(const string& Key, const int Value)
	{
		if (Monitor.IsValid())
		{
			Monitor->LogSceneInfo(Key, Value);
		}
	}

	void BaseGPMSDK::LogSceneInfo(const string& Key)
	{
		if (Monitor.IsValid())
		{
			Monitor->LogSceneInfo(Key);
		}
	}
}




