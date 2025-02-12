// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GPMMonitor.h"
#include <string>
using std::string;
/**
 * 
 */
namespace GMSDK
{
	class GPM_API BaseGPMSDK
	{
	public:
		BaseGPMSDK();
		~BaseGPMSDK();
		int GraphicLevel();
		void LogSceneStart(const string& SceneName);
		void LogSceneLoaded();
		void LogSceneEnd();
		void LogGlobalInfo(const string& Key, const string& Value);
		void LogGlobalInfo(const string& Key, const int Value);
		void LogGlobalInfo(const string& Key);
		void LogSceneInfo(const string& Key, const string& Value);
		void LogSceneInfo(const string& Key, const int Value);
		void LogSceneInfo(const string& Key);
	private:
		TWeakObjectPtr<UGPMMonitor> Monitor = nullptr;
	};

}
