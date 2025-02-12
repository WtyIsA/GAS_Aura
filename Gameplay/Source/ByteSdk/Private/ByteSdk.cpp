// Copyright Epic Games, Inc. All Rights Reserved.

#include "ByteSdk.h"

#include "Sdk/SdkInterface.h"
#include "Android/ByteSdkAndroidInterface.h"
#include "Android/ByteRtcInterface.h"
#define LOCTEXT_NAMESPACE "FByteSdkModule"

void FByteSdkModule::StartupModule()
{
	m_pSdkInterface = nullptr;
	m_pVoiceInterface = nullptr;
#if PLATFORM_ANDROID
	m_pSdkInterface = new FByteSdkAndroidInterface();
	UE_LOG(LogTemp,Display,TEXT("bytesdk new FAndroidSdk %p"), m_pSdkInterface);
    FSdkInterface::SetSdkInterface(m_pSdkInterface);
	m_pVoiceInterface = new FByteRtcInterface();
	FVoiceInterface::SetSdkInterface(m_pVoiceInterface);
#endif
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FByteSdkModule::ShutdownModule()
{
	if(m_pSdkInterface)
		delete m_pSdkInterface;
	m_pSdkInterface = nullptr;
	if(m_pVoiceInterface)
		delete m_pVoiceInterface;
	m_pVoiceInterface = nullptr;
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FByteSdkModule, ByteSdk)