﻿// Copyright © 2014-2021 YouMe. All Rights Reserved.

#include "YouMeTalk.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "YouMeTalkRtcInterface.h"

#define LOCTEXT_NAMESPACE "FYouMeTalkModule"
FYouMeTalkRtcInterface* FYouMeTalkModule::m_pYoumeVoiceInterface = nullptr;
void FYouMeTalkModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
#if !UE_SERVER
	m_pYoumeVoiceInterface = new FYouMeTalkRtcInterface();
	FVoiceInterface::SetSdkInterface(m_pYoumeVoiceInterface);

#endif
}

void FYouMeTalkModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if(m_pYoumeVoiceInterface)
		delete m_pYoumeVoiceInterface;
	m_pYoumeVoiceInterface = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FYouMeTalkModule, YouMeTalk)
