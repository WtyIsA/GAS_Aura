#include "EvolutionOutput.h"

#include "../Public/LuaManger.h"

void FEvolutionOutputWarn::Serialize( const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category )
{
	if (!IsInGameThread() || FUObjectThreadContext::Get().IsRoutingPostLoad)
		return;
	FFeedbackContextAnsi::Serialize(V,Verbosity,Category);
	if (bLogToGameServer && Verbosity <= eLogLevel)
	{
	    FString tempStr = V;
		FString LevelName = ToString(Verbosity);
		tempStr = TEXT("[") + LevelName + TEXT("] ") + tempStr;
		ULuaManger::Get()->LuaReportLogToGameServer(tempStr);
	}
}

FEvolutionOutputWarn* FEvolutionOutputWarn::Get()
{
	static FEvolutionOutputWarn Singleton;
	return &Singleton;
}

FFeedbackContext* FEvolutionOutputWarn::GetAsFeedbackContext()
{
	return FEvolutionOutputWarn::Get();
}

void FEvolutionOutputError::Serialize( const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category )
{
	if (!IsInGameThread() || FUObjectThreadContext::Get().IsRoutingPostLoad)
		return;
	FOutputDeviceAnsiError::Serialize(V,Verbosity,Category);
	if (bLogToGameServer && Verbosity <= eLogLevel)
	{
	    FString tempStr = V;
		FString LevelName = ToString(Verbosity);
		tempStr = TEXT("[") + LevelName + TEXT("] ") + tempStr;
		ULuaManger::Get()->LuaReportLogToGameServer(tempStr);
	}
}

FEvolutionOutputError* FEvolutionOutputError::Get()
{
	static FEvolutionOutputError Singleton;
	return &Singleton;
}

FOutputDeviceError* FEvolutionOutputError::GetAsOutputDeviceError()
{
	return FEvolutionOutputError::Get();
}

void FEvolutionOutputReportGs::Serialize( const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category )
{
	if (!IsInGameThread() || FUObjectThreadContext::Get().IsRoutingPostLoad)
		return;
	if (bLogToGameServer && Verbosity <= eLogLevel)
	{
	    FString tempStr = V;
		FString LevelName = ToString(Verbosity);
		tempStr = TEXT("[") + LevelName + TEXT("] ") + tempStr;
		ULuaManger::Get()->LuaReportLogToGameServer(tempStr);
	}
}

FEvolutionOutputReportGs* FEvolutionOutputReportGs::Get()
{
	static FEvolutionOutputReportGs Singleton;
	return &Singleton;
}

FOutputDevice* FEvolutionOutputReportGs::GetAsFOutputDevice()
{
	return FEvolutionOutputReportGs::Get();
}

bool FEvolutionOutputReportGs::CanBeUsedOnMultipleThreads() const
{
	return true;
}