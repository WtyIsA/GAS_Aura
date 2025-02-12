#include "FForkProcess.h"
#include "IPlatformFilePak.h"
#include "HAL/PlatformApplicationMisc.h"
#include "HAL/FileManager.h"
#include "HAL/ThreadHeartBeat.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformMemory.h"
#include "UnrealEngine.h"
#include "Misc/CoreDelegates.h"
#include "Misc/Fork.h"
#include "HttpModule.h"
#include "HttpManager.h"
#include "HAL/ThreadManager.h"
#include "ProfilingDebugging/CsvProfiler.h"

bool FForkProcess::bForkAlreadyAttempted = false;
bool FForkProcess::bShutdownWithExitCodeToCloseParent = false;

FDelegateHandle FForkProcess::OnEndFrameHandle;

namespace FForkProcessInternal
{
	static bool CVar_Server_MultithreadTaskGraph = true;

	static bool CVar_Server_ReloadPakReaders = true;

	static FDelegateHandle OnHotfixTaskGraph;

	/** Delegate for resetting the task graph threading strategy */
	void OnEndFrameHotfixTaskGraph();

	/**
	 * Are we doing a real fork and generating child processes from the master who received the -WaitAndFork commandline.
	 * Will be true on child processes even if they do not receive the WaitAndFork command.
	 */
	bool IsRealForkRequested()
	{
		// We cache the value since only the master process will receive WaitAndFork
		static const bool bRealForkRequested = FParse::Param(FCommandLine::Get(), TEXT("WaitAndFork"));
		return bRealForkRequested;
	}

	/**
	* Fake Forking is when we run the Fork codepath without actually duplicating the process.
	* Can be used on platforms that do not support forking or to simply debug the fork behavior without attaching to the new process.
	* Note: The master process is considered a child process after the Fork event.
	*/
	bool IsFakeForking()
        {
#if UE_SERVER
        	const bool bRealForkRequested = FForkProcessInternal::IsRealForkRequested();
        	if (bRealForkRequested)
        	{
        		return false;
        	}

        	const bool bNoFakeForking = FParse::Param(FCommandLine::Get(), TEXT("NoFakeForking"));
        	if (bNoFakeForking)
        	{
        		return false;
        	}

        	// Default to have dedicated servers simulate the fork since it's closer to the live environment.
        	return true;
#else
	        return false;
#endif
        }
}

#ifndef WAIT_AND_FORK_PARENT_SHUTDOWN_EXIT_CODE
	#define WAIT_AND_FORK_PARENT_SHUTDOWN_EXIT_CODE 0
#endif


void FForkProcess::ForkIfRequested(UWorld* World)
{
	// Only attempt to fork exactly once. Child instances should not fork. This is an important protection against accidental fork bombs.
	if (bForkAlreadyAttempted)
	{
		return;
	}

	bForkAlreadyAttempted = true;

	check(World);

	// Should this process be duplicated and create child processes
	const bool bRealFork = FForkProcessInternal::IsRealForkRequested();

	// Should we run the Fork codepath without duplicating the process.
	const bool bFakeFork = !bRealFork && FForkProcessInternal::IsFakeForking();

	if (bRealFork || bFakeFork)
	{
		// Do we turn off and restart the GameNetDriver after the fork
		const bool bRestartNetDriver = bRealFork;
		if (bRestartNetDriver)
		{
			GEngine->ShutdownWorldNetDriver(World);
		}

		// Flush the http manager to make sure there are no outstanding http requests when we fork
		FHttpManager& HttpManager = FHttpModule::Get().GetHttpManager();
		HttpManager.OnBeforeFork();

		// Do we reset the pak readers on child processes
		const bool bResetPakReaders = bRealFork;

		TArray<FString> AllPakFiles;
		TArray<FString> PakFolders;
		FPakPlatformFile* PakPlatformFile = nullptr;

		if (bResetPakReaders)
		{
			PakPlatformFile = (FPakPlatformFile*)(FPlatformFileManager::Get().FindPlatformFile(FPakPlatformFile::GetTypeName()));
			if (PakPlatformFile == nullptr)
			{
				UE_LOG(LogEngine, Fatal, TEXT("You cannot use WaitAndFork without using pak files."));
			}

			PakPlatformFile->GetPakFolders(FCommandLine::Get(), PakFolders);
			for (const FString& PakFolder : PakFolders)
			{
				TArray<FString> PakFiles;
				IFileManager::Get().FindFiles(PakFiles, *PakFolder, TEXT(".pak"));
				for (const FString& PakFile : PakFiles)
				{
					AllPakFiles.Add(PakFolder / PakFile);
				}
			}

			if (AllPakFiles.Num() == 0)
			{
				UE_LOG(LogEngine, Fatal, TEXT("WaitAndFork cannot proceed without finding pak files."));
			}

			if (!FForkProcessInternal::CVar_Server_ReloadPakReaders)
			{
				for (const FString& PakFile : AllPakFiles)
				{
					if (!FCoreDelegates::OnUnmountPak.Execute(PakFile))
					{
						UE_LOG(LogEngine, Fatal, TEXT("WaitAndFork failed to unmount pak %s."), *PakFile);
					}
				}
			}
		}

		// ******** The fork happens here! ********
		FPlatformProcess::EWaitAndForkResult Result = FPlatformProcess::EWaitAndForkResult::Error;
		if (bRealFork)
		{
			Result = FPlatformProcess::WaitAndFork();
		}
		else if (bFakeFork)
		{
			// The master process becomes a child when fake forking.
			Result = FPlatformProcess::EWaitAndForkResult::Child;
			FForkProcessHelper::SetIsForkedChildProcess();
		}
		// ******** The fork happened! This is now either the parent process exiting gracefully (or erroring) or the new child process starting up ********

		if (bResetPakReaders && !FForkProcessInternal::CVar_Server_ReloadPakReaders)
		{
			for (const FString& PakFile : AllPakFiles)
			{
				if (!FCoreDelegates::MountPak.Execute(PakFile, 0))
				{
					UE_LOG(LogEngine, Fatal, TEXT("WaitAndFork failed to mount pak %s."), *PakFile);
				}
			}
		}
		UE_LOG(LogHAL, Warning, TEXT("Finish WaitAndFork 11111"));
		// Restart the http manager
		HttpManager.OnAfterFork();

		if (Result == FPlatformProcess::EWaitAndForkResult::Error)
		{
			UE_LOG(LogEngine, Fatal, TEXT("There was an error attempting to fork. Are you sure you launched with -nothreading?"));
		}
		else if (Result == FPlatformProcess::EWaitAndForkResult::Parent)
		{
			// The parent exited gracefully
		}
		else if (Result == FPlatformProcess::EWaitAndForkResult::Child)
		{
			if (bResetPakReaders && FForkProcessInternal::CVar_Server_ReloadPakReaders)
			{
				PakPlatformFile->ReloadPakReaders();
			}

			FGameThreadHitchHeartBeat::Get().Restart();

			FURL::StaticExit();
			FURL::StaticInit();

			// Here we do some post-fork setup for the game instance. 

			OnEndFrameHandle = FCoreDelegates::OnEndFrame.AddStatic(FForkProcess::OnEndFramePostFork);
			// Here we reinitialize the IOnlineTitleFile dispatcher
			// if (bRestartNetDriver)
			// {
			// 	FURL DefaultURL;
			// 	World->Listen(DefaultURL);
			// }

			// We don't want child processes to write to all the static memory we are sharing during shutdown, so we will exit before the
			// UObject system is torn down. For this reason, BeginDestroy/destructors for these objects will not be called for child processes.
			if (bRealFork)
			{
				UE_LOG(LogEngine, Warning, TEXT("FCoreDelegates::OnPreExit.AddStatic."));
				FCoreDelegates::OnPreExit.AddStatic([]()
				{
					UE_LOG(LogEngine, Display, TEXT("Forked child process flushing http requests before exiting."));
					FHttpManager& LocalHttpManager = FHttpModule::Get().GetHttpManager();
					LocalHttpManager.Flush(true);

					uint8 ExitCode = bShutdownWithExitCodeToCloseParent ? WAIT_AND_FORK_PARENT_SHUTDOWN_EXIT_CODE : 0;
					UE_LOG(LogEngine, Display, TEXT("Forked child process exiting cleanly with exit code %d."), ExitCode);
					if (GLog)
					{
						GLog->Flush();
					}
					
					FPlatformMisc::RequestExitWithStatus(true, ExitCode);
					UE_LOG(LogEngine, Warning, TEXT("FCoreDelegates::OnPreExit Shutdown"));
				});
			}
		}
		else
		{
			UE_LOG(LogEngine, Fatal, TEXT("Unknown return value from WaitAndFork. Result:%d"), (uint8)Result);
		}
	}

    // Here we initialize perf counters for the dedicated server game instance
}

void FForkProcess::MarkShouldCloseParentProcessWhenShuttingDown()
{
	// Only forked children of a process may request to shutdown a parent process
	if (FForkProcessHelper::IsForkedChildProcess())
	{
		bShutdownWithExitCodeToCloseParent = true;
	}
}

bool FForkProcess::ShutdownProcessAtSessionEnd()
{
	return FForkProcessHelper::IsForkedChildProcess() && !FForkProcessInternal::IsFakeForking();
}

void FForkProcess::OnEndFramePostFork()
{
	// This delegate is only called once
	FCoreDelegates::OnEndFrame.Remove(OnEndFrameHandle);
	OnEndFrameHandle.Reset();

	if( FForkProcessHelper::SupportsMultithreadingPostFork() )
	{
		// Convert forkable threads into real threads
		FForkProcessHelper::OnForkingOccured();

		if( FForkProcessInternal::CVar_Server_MultithreadTaskGraph )
		{
			FTaskGraphInterface::Shutdown();	
			FTaskGraphInterface::Startup(0);
			FTaskGraphInterface::Get().AttachToThread(ENamedThreads::GameThread);
			UE_LOG(LogHAL, Warning, TEXT("OnEndFramePostFork Restart TaskGraph"));
		}
	}

	//TODO: Add a CoreDelegate::OnEndFramePostFork so systems can hook into it instead of us telling them we forked.

	FHttpModule::Get().GetHttpManager().OnEndFramePostFork();

#if CSV_PROFILER
	FCsvProfiler::Get()->OnEndFramePostFork();
#endif
}

/** This CVar is used to flip back to unmounting the pak files before we fork and having the children re-mount
  * these pak files. This was causing a hit to USS so a new way is to simply allow the children to reload the
  * pak readers which have a handle on the low level file handle
  */
static FAutoConsoleVariableRef CVarServerReloadPakReaders(
	TEXT("Server.ReloadPakReaders"),
	FForkProcessInternal::CVar_Server_ReloadPakReaders,
	TEXT("Has child process re-create the pak readers over unmounting then remounting pak files"),
	ECVF_Default);