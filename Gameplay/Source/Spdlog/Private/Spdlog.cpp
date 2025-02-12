// Fill out your copyright notice in the Description page of Project Settings.

#include "Spdlog.h"

#include "Spdlogger.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FSpdlogModule"

void FSpdlogModule::StartupModule()
{
}

void FSpdlogModule::ShutdownModule()
{
	USpdlogger::CloseAll();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSpdlogModule, Spdlog);