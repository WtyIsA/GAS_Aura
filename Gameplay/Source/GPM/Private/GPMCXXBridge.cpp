// Fill out your copyright notice in the Description page of Project Settings.


#include "GPMCXXBridge.h" 
#if (PLATFORM_ANDROID || PLATFORM_IOS) 
#include "cxx_api.h"
#endif

namespace GMSDK
{
	GPMCXXBridge::GPMCXXBridge()
	{
	}

	GPMCXXBridge::~GPMCXXBridge()
	{
	}

	void GPMCXXBridge::LogSceneStart(const string& Type, const string& Name)
	{
#if (PLATFORM_ANDROID || PLATFORM_IOS) 
		GPM_CXX_LogSceneStart(Type.c_str(), Name.c_str());
#endif
	}

	void GPMCXXBridge::LogSceneLoaded(const string& Type, const string& Name)
	{
#if (PLATFORM_ANDROID || PLATFORM_IOS) 
		GPM_CXX_LogSceneLoaded(Type.c_str(), Name.c_str());
#endif
	}

	void GPMCXXBridge::LogSceneEnd(const string& Type, const string& Name)
	{
#if (PLATFORM_ANDROID || PLATFORM_IOS) 
		GPM_CXX_LogSceneEnd(Type.c_str(), Name.c_str(),true);
#endif
	}

	void GPMCXXBridge::LogFrameEnd()
	{
#if (PLATFORM_ANDROID || PLATFORM_IOS) 
		GPM_CXX_LogFrameEnd();
#endif
	}

	void GPMCXXBridge::LogCPUUsage()
	{
#if (PLATFORM_ANDROID || PLATFORM_IOS) 
		GPM_CXX_LogCPUUsage();
#endif
	}

	void GPMCXXBridge::LogMemoryUsage()
	{
#if (PLATFORM_ANDROID || PLATFORM_IOS) 
		GPM_CXX_LogMemoryUsage();
#endif
	}

	void GPMCXXBridge::LogGlobalInfo(const string& Type, const string& Key, const string& Value)
	{
#if (PLATFORM_ANDROID || PLATFORM_IOS) 
		GPM_CXX_LogGlobalInfoStr(Type.c_str(), Key.c_str(), Value.c_str());
#endif
	}

	void GPMCXXBridge::LogGlobalInfo(const string& Type, const string& Key, const int Value)
	{
#if (PLATFORM_ANDROID || PLATFORM_IOS) 
		GPM_CXX_LogGlobalInfoInt(Type.c_str(), Key.c_str(), Value);
#endif
	}

	void GPMCXXBridge::LogGlobalInfo(const string& Type, const string& Key)
	{
#if (PLATFORM_ANDROID || PLATFORM_IOS) 
		GPM_CXX_LogGlobalInfoKey(Type.c_str(), Key.c_str());
#endif
	}

	void GPMCXXBridge::LogSceneInfo(const string& Type, const string& Name, const string& Key, const string& Value)
	{
#if (PLATFORM_ANDROID || PLATFORM_IOS) 
		GPM_CXX_LogSceneInfoStr(Type.c_str(), Name.c_str(), Key.c_str(), Value.c_str());
#endif
	}

	void GPMCXXBridge::LogSceneInfo(const string& Type, const string& Name, const string& Key, const int Value)
	{
#if (PLATFORM_ANDROID || PLATFORM_IOS) 
		GPM_CXX_LogSceneInfoInt(Type.c_str(), Name.c_str(), Key.c_str(), Value);
#endif
	}

	void GPMCXXBridge::LogSceneInfo(const string& Type, const string& Name, const string& Key)
	{
#if (PLATFORM_ANDROID || PLATFORM_IOS) 
		GPM_CXX_LogSceneInfoKey(Type.c_str(), Name.c_str(), Key.c_str());
#endif
	}

	int GPMCXXBridge::RequestGraphicLevel()
	{
#if (PLATFORM_ANDROID || PLATFORM_IOS) 
		return GPM_CXX_RequestGraphicLevel();
#else
		return 0;
#endif
	}
}

