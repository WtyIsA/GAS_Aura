// Copyright Epic Games, Inc. All Rights Reserved.

#include "MainModule.h"

#include "LuaManger.h"
#include "UnLuaBase.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"

#define LOCTEXT_NAMESPACE "FMainModule"

void FMainModule::StartupModule()
{
}

void FMainModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMainModule, Main)