// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
class FSdkInterface;
class FVoiceInterface;
class FByteSdkModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	FSdkInterface*		m_pSdkInterface;
	FVoiceInterface*	m_pVoiceInterface;
};
