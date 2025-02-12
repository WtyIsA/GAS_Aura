#pragma once
#include "HAL/FeedbackContextAnsi.h"
#include "Misc/OutputDeviceAnsiError.h"

class MAIN_API FEvolutionOutputWarn : FFeedbackContextAnsi
{
public:
	bool bLogToGameServer = false;
	ELogVerbosity::Type eLogLevel = ELogVerbosity::NoLogging;

	virtual void Serialize( const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category ) override;

	static FEvolutionOutputWarn* Get();
	static FFeedbackContext* GetAsFeedbackContext();
};

class MAIN_API FEvolutionOutputError : FOutputDeviceAnsiError
{
public:
	bool bLogToGameServer = false;
	ELogVerbosity::Type eLogLevel = ELogVerbosity::NoLogging;

	virtual void Serialize( const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category ) override;
	
	static FEvolutionOutputError* Get();
	static FOutputDeviceError* GetAsOutputDeviceError();
};

class MAIN_API FEvolutionOutputReportGs : FFeedbackContext
{
public:
	bool bLogToGameServer = false;
	ELogVerbosity::Type eLogLevel = ELogVerbosity::NoLogging;

	virtual void Serialize( const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category ) override;
	
	static FEvolutionOutputReportGs* Get();
	static FOutputDevice* GetAsFOutputDevice();

	virtual bool CanBeUsedOnMultipleThreads() const override;
};
