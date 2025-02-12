#include "FPSManager.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
UFPSManager* UFPSManager::handle = nullptr;
UGameInstance* UFPSManager::gameInsHandle = nullptr;

void UFPSManager::Init(uint8 optimizeFps)
{
	OptimizeFps = optimizeFps;
	OnSlateUserInteractionHandle = FSlateApplication::Get().GetLastUserInteractionTimeUpdateEvent().AddUObject(
		handle, &UFPSManager::OnSlateUserInteraction);
}

UFPSManager* UFPSManager::Get(UGameInstance* pGameIns)
{
	if (handle == nullptr)
	{
		gameInsHandle = pGameIns;
		handle = NewObject<UFPSManager>(pGameIns);
		handle->AddToRoot();
	}
	return handle;
}

void UFPSManager::Tick()
{
	if (FSlateApplication::Get().GetCurrentTime() - lastInputTime > CheckTime)
	{
		SetCVarMaxFps(UKismetMathLibrary::Min(targetMaxFps, OptimizeFps));
	}
	else
	{
		SetCVarMaxFps(targetMaxFps);
	}
}

void UFPSManager::Free()
{
	if (handle)
	{
		handle->RemoveFromRoot();
		FSlateApplication::Get().GetLastUserInteractionTimeUpdateEvent().Remove(handle->OnSlateUserInteractionHandle);
		FCoreDelegates::OnBeginFrame.Remove(handle->OnBeginFrameHandle);
		handle = nullptr;
	}
}

void UFPSManager::SetTargetMaxFps(uint8 targetFps)
{
	targetMaxFps = targetFps;
	SetCVarMaxFps(targetFps);
}

void UFPSManager::SetEnable(bool bEnable)
{
	if (bEnable)
	{
		FCoreDelegates::OnBeginFrame.AddUObject(handle, &UFPSManager::Tick);
	}
	else
	{
		FCoreDelegates::OnBeginFrame.Remove(OnBeginFrameHandle);
	}
}

void UFPSManager::SetCVarMaxFps(uint8 targetFps)
{
	if (targetFps != curTargetFps)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(gameInsHandle, EGetWorldErrorMode::ReturnNull);
		if (World)
		{
			APlayerController* TargetPC = World->GetFirstPlayerController();
			if (TargetPC)
			{
				TargetPC->ConsoleCommand(FString::Printf(TEXT("t.MaxFPS %d"), targetFps),false);
			}
			else
			{
				GEngine->Exec(World, *FString::Printf(TEXT("t.MaxFPS %d"), targetFps));
			}
			//UKismetSystemLibrary::ExecuteConsoleCommand(gameInsHandle, FString::Printf(TEXT("t.MaxFPS %d"), targetFps));
			curTargetFps = targetFps;
		}
	}
}

void UFPSManager::OnSlateUserInteraction(double CurrSlateInteractionTime)
{
	lastInputTime = CurrSlateInteractionTime;
}
