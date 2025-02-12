// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolingManager.h"

#include "LuaManger.h"
#include "Engine/World.h"
#include "Engine/NetDriver.h"


void FPoolingManagerData::WaitRecycleToReady()
{
	for (auto Actor : WaitRecycleActors)
	{
		ReadyActors.Add(Actor);
	}
	WaitRecycleActors.Empty();
}

void FPoolingManagerData::ClearAll()
{
	ReadyActors.Empty();
	WaitRecycleActors.Empty();
}

APoolingManager::APoolingManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APoolingManager::BeginPlay()
{
	Super::BeginPlay();
	SetMinGCClusterSize(1);
	AddToRoot();
	bServer = IsNetMode(NM_DedicatedServer);
	for (auto Data : PoolingData)
	{
		Data.Value->ActorClass = Data.Key;
		for (int i = 0; i < Data.Value->InitCount; ++i)
		{
			FActorParamFunctionDelegate InitFunctionDelegate;
			RecycleActor(GetActor(Data.Key, InitFunctionDelegate),InitFunctionDelegate);
		}
	}
}

void APoolingManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	RemoveFromRoot();
}

bool APoolingManager::CanBeClusterRoot() const
{
	return true;
}

void APoolingManager::InitPoolActor(AActor* PoolActor) const
{
	if (bServer)
	{
		PoolActor->SetReplicates(true);
	}
	PoolActor->SetActorLocation(InitPosition);
	PoolActor->SetActorHiddenInGame(false);
	PoolActor->SetActorEnableCollision(true);
	PoolActor->SetActorTickEnabled(true);
}

FPoolingManagerData* APoolingManager::GetPoolingManagerData(const TSubclassOf<AActor> ActorClass)
{
	FPoolingManagerData** Data = PoolingData.Find(ActorClass);
	if (Data == nullptr)
	{
		FPoolingManagerData* temp = new FPoolingManagerData();
		Data = &temp;
		PoolingData.Add(ActorClass, temp);
	}
	return *Data;
}

void APoolingManager::SetMinGCClusterSize(int32 Size)
{
	if (IConsoleVariable* CVarDebugCanvasVisible = IConsoleManager::Get().FindConsoleVariable(
		TEXT("gc.MinGCClusterSize")))
	{
		CVarDebugCanvasVisible->Set(Size);
	}
}

void APoolingManager::OnLevelActorDeleted(AActor* InActor)
{
	if (!IsValid(InActor))
	{
		return;
	}
	FPoolingManagerData** Data = PoolingData.Find(InActor->GetClass());
	if (Data != nullptr)
	{
		if (FPoolingManagerData* PoolingMangerData = *Data)
		{
			if (PoolingMangerData->ReadyActors.Contains(InActor))
			{
				PoolingMangerData->ReadyActors.Remove(InActor);
				UE_LOG(LogTemp, Warning, TEXT("Pool Actor Destory Name:%s Pos:%s"), *InActor->GetName(),
				       *InActor->GetActorLocation().ToString());
			}
		}
	}
}

void APoolingManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (auto Data : PoolingData)
	{
		Data.Value->WaitRecycleToReady();
	}
}

AActor* APoolingManager::GetActor(const TSubclassOf<AActor> ActorClass, const FActorParamFunctionDelegate& InitFunction)
{
	if (ActorClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APoolingManager::GetActor ActorClass was null"));
		return nullptr;
	}
	FPoolingManagerData* Data = GetPoolingManagerData(ActorClass);
	AActor* PoolActor = nullptr;
	if (Data->ReadyActors.Num() > 0)
	{
		for (auto It = Data->ReadyActors.CreateConstIterator(); It; ++It)
		{
			PoolActor = *It;
			if (IsValid(PoolActor))
			{
				UE_LOG(LogTemp, Warning, TEXT("PoolingManager::GetActor:%s"), *PoolActor->GetName());
				break;
			}
			else
			{
				if (Data->ActorClass)
				{
					UE_LOG(LogTemp, Warning, TEXT("APoolingManager:: ReadyActors have null:%s"),
					       *Data->ActorClass->GetName());
				}
			}
		}
		if (!IsValid(PoolActor))
		{
			if (Data->ActorClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("APoolingManager::GetActor PoolActor was null:%s"),
				       *Data->ActorClass->GetName());
			}
			if (PoolActor)
			{
				Data->ReadyActors.Remove(PoolActor);
			}
			return nullptr;
		}
		Data->ReadyActors.Remove(PoolActor);
	}
	else
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		PoolActor = GetWorld()->SpawnActor<AActor>(ActorClass, InitPosition, FRotator(0, 0, 0), SpawnParameters);
		if (PoolActor==nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("APoolingManager::GetActor SpawnActor Fail:%s"),
					   *ActorClass->GetName());
			return nullptr;
		}
		const FActorParamFunctionDelegate* Function = InitFuntionsMap.Find(ActorClass);
		if (Function != nullptr)
		{
			Function->ExecuteIfBound(PoolActor);
		}
		///PoolActor->SetCanBeInCluster(true);
		//ULuaManger::AddActorToLevelCluster(PoolActor);
	}
	if(PoolActor)
	{
		InitFunction.ExecuteIfBound(PoolActor);
		InitPoolActor(PoolActor);
	}
	return PoolActor;
}

void APoolingManager::SetInitFunction(TSubclassOf<AActor> ActorClass, const FActorParamFunctionDelegate& Function)
{
	InitFuntionsMap.Add(ActorClass, Function);
}

void APoolingManager::RecycleActor(AActor* Actor,const FActorParamFunctionDelegate& RecycleActorFunction)
{
	if (Actor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APoolingManager::RecycleActor Actor was null"));
		return;
	}
	FPoolingManagerData** Data = PoolingData.Find(Actor->GetClass());
	if (Data == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("APoolingManager::RecycleActor FPoolingManagerData was null"));
		return;
	}
	FPoolingManagerData* PoolingMangerData = *Data;
	if (PoolingMangerData->ReadyActors.Num() + PoolingMangerData->WaitRecycleActors.Num() >= PoolingMangerData->
		MaxCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("APoolingManager::RecycleActor MaxCount %s"), *Actor->GetName());
		Actor->Destroy();
	}
	else
	{
		PoolingMangerData->WaitRecycleActors.Add(Actor);
		if (bServer)
		{
			GetWorld()->GetNetDriver()->NotifyActorDestroyed(Actor);
			Actor->SetReplicates(false);
		}
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);
		Actor->SetActorTickEnabled(false);
		Actor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Actor->SetActorLocation(InitPosition);
		RecycleActorFunction.ExecuteIfBound(Actor);
		UE_LOG(LogTemp, Warning, TEXT("PoolingManager::RecycleActor:%s"), *Actor->GetName());
	}
}

void APoolingManager::ClearAll()
{
	for (auto Data : PoolingData)
	{
		Data.Value->ClearAll();		
	}
	PoolingData.Empty();
}
