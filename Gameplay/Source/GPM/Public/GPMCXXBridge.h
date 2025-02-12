// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <string>
using std::string;
/**
 * 
 */
namespace GMSDK
{
	class GPM_API GPMCXXBridge
	{
	public:
		GPMCXXBridge();
		~GPMCXXBridge();
		static void LogSceneStart(const string& Type, const string& Name);
		static void LogSceneLoaded(const string& Type, const string& Name);
		static void LogSceneEnd(const string& Type, const string& Name);
		static void LogFrameEnd();
		static void LogCPUUsage();
		static void LogMemoryUsage();
		static void LogGlobalInfo(const string& Type, const string& Key, const string& Value);
		static void LogGlobalInfo(const string& Type, const string& Key, const int Value);
		static void LogGlobalInfo(const string& Type, const string& Key);
		static void LogSceneInfo(const string& Type, const string& Name, const string& Key, const string& Value);
		static void LogSceneInfo(const string& Type, const string& Name, const string& Key, const int Value);
		static void LogSceneInfo(const string& Type, const string& Name, const string& Key);
		static int RequestGraphicLevel();
	};
}

