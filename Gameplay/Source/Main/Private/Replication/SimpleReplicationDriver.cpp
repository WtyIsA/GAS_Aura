#include "SimpleReplicationDriver.h"

#include <Actor.h>

#include "EngineUtils.h"
#include "LuaManger.h"
#include "ReplicationGraphTypes.h"
#include "Engine/ChildConnection.h"
#include "SelfPlayerController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/ActorChannel.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "Misc/CoreDelegates.h"
#include "Net/RepLayout.h"

DEFINE_LOG_CATEGORY( LogSimpleRepDriver );

static const uint16 NEAR_DISTANCE = 0;
static const uint16 MIDDLE_DISTANCE = 1;
static const uint16 FAR_DISTANCE = 2;
static const uint16 MAX_DISTANCE = 2;

void FGrid::DumpInfo()
{
	if(NetConnections.Num())
	{
		UE_LOG(LogSimpleRepDriver, Log, TEXT("USimpleReplicationDriver Dump Grid X=%d,Y=%d Connection"),X,Y);
		int32 Index = 0;
		for(TWeakObjectPtr<UNetConnection> ConnectionPtr : NetConnections)
		{
			++Index;
			if(ConnectionPtr.IsValid() == false)
			{
				UE_LOG(LogSimpleRepDriver, Log, TEXT("Dump Connection Index=%d Is Not Valid"),Index);
				continue;
			}
			UE_LOG(LogSimpleRepDriver, Log, TEXT("Dump Connection Index=%d Name=%s Viewtarget=%s"),Index,*GetNameSafe(ConnectionPtr.Get()),*GetNameSafe((ConnectionPtr->ViewTarget)));
		}
	}
	for(int32 i=0;i<ActorsArray.Num();++i)
	{
		TSet<TWeakObjectPtr<AActor>> Actors = ActorsArray[i];
		if(Actors.Num())
		{
			UE_LOG(LogSimpleRepDriver, Log, TEXT("USimpleReplicationDriver Grid X=%d,Y=%d DistanceLevel=%d,ActorNum=%d"),X,Y,i,Actors.Num());
			int32 Index = 0;
			for(TWeakObjectPtr<AActor> ActorPtr : Actors)
			{
				++Index;
				if(ActorPtr.IsValid() == false)
				{
					UE_LOG(LogSimpleRepDriver, Log, TEXT("Dump Actor Index=%d Is Not Valid"),Index);
					continue;
				}
				UE_LOG(LogSimpleRepDriver, Log, TEXT("Dump Actor Index=%d Name=%s"),Index,*GetNameSafe(ActorPtr.Get()));
			}
		}
	}
}

USimpleReplicationDriver::USimpleReplicationDriver():QuadTree(FBox2D(FVector2D(-409600, -409600), FVector2D(409600, 409600)), 2000),TinderQuadTree(FBox2D(FVector2D(-409600, -409600), FVector2D(409600, 409600)), 2000)
{
	CellSize = 500.0f;
	MapSize = 200000.0f;
	HalfMapSize = MapSize * 0.5f;
	RelevantCheckTime = 1.0f;
	MaxCellIndex = FMath::CeilToInt(MapSize/CellSize);
	NearNetCullDistance = 2000.0f;
	NearNetCullDistanceSquared = NearNetCullDistance * NearNetCullDistance;
	MiddleCullDistance = 3000.0f;
	MiddleNetCullDistanceSquared = MiddleCullDistance * MiddleCullDistance;
	FarCullDistance = 5000.0f;
	FarNetCullDistanceSquared = FarCullDistance * FarCullDistance;
	GridDiffs.Add(FMath::CeilToInt(NearNetCullDistance/CellSize));
	GridDiffs.Add(FMath::CeilToInt(MiddleCullDistance/CellSize));
	GridDiffs.Add(FMath::CeilToInt(FarCullDistance/CellSize));

	Grids.SetNum(MaxCellIndex + 1);
	for(int32 i = 0;i<=MaxCellIndex;++i)
	{
		TArray<FGrid*> GridCellArray;
		for(int32 j=0;j<=MaxCellIndex;++j)
		{
			FGrid* NewCell = new FGrid(MAX_DISTANCE + 1,i,j);
			GridCellArray.Emplace(NewCell);
		}
		Grids[i] = GridCellArray;
	}
}

void USimpleReplicationDriver::SetRepDriverWorld(UWorld* InWorld)
{
	if (World && NetDriver)
	{
		FCoreDelegates::OnBeginFrame.Remove(NetDriver->TickDispatchDelegateHandle);
		FCoreDelegates::OnEndFrame.Remove(NetDriver->PostTickDispatchDelegateHandle);
		FCoreDelegates::OnEndFrame.Remove(NetDriver->TickFlushDelegateHandle);
		FCoreDelegates::OnEndFrame.Remove(NetDriver->PostTickFlushDelegateHandle);
	}
	
	World = InWorld;
}

void USimpleReplicationDriver::InitForNetDriver(UNetDriver* InNetDriver)
{
	NetDriver = InNetDriver;
	if (World && NetDriver)
	{
		World->OnTickDispatch ().Remove(NetDriver->TickDispatchDelegateHandle);
		World->OnPostTickDispatch().Remove(NetDriver->PostTickDispatchDelegateHandle);
		World->OnTickFlush().Remove(NetDriver->TickFlushDelegateHandle);
		World->OnPostTickFlush().Remove(NetDriver->PostTickFlushDelegateHandle);

		NetDriver->TickDispatchDelegateHandle = FCoreDelegates::OnBeginFrame.AddLambda([this]() { this->NetDriver->TickDispatch(FApp::GetDeltaTime()); });
		NetDriver->PostTickDispatchDelegateHandle = FCoreDelegates::OnEndFrame.AddLambda([this]() { this->NetDriver->PostTickDispatch(); });
		NetDriver->TickFlushDelegateHandle = FCoreDelegates::OnEndFrame.AddLambda([this]() { this->NetDriver->TickFlush(FApp::GetDeltaTime()); });
		NetDriver->PostTickFlushDelegateHandle = FCoreDelegates::OnEndFrame.AddLambda([this]() { this->NetDriver->PostTickFlush(); });
		GEngine->Exec(World, TEXT("net.IpNetDriverUseReceiveThread 1"));
		GEngine->Exec(World, TEXT("net.WakeupGameThreadWhenReceivedPackets 1"));

		FCoreDelegates::OnObjectNetPropertyChanged.AddUObject(this, &USimpleReplicationDriver::OnObjectNetPropertyChanged);
		FCoreDelegates::OnGameThreadWakeup.AddUObject(this, &USimpleReplicationDriver::OnGameThreadWakeup);
	}
	
	for (UNetConnection* ClientConnection : NetDriver->ClientConnections)
	{
		AddClientConnection(ClientConnection);
	}
	ULuaManger::Get()->OnReplicateActorFail.AddUObject(this,&USimpleReplicationDriver::OnReplicateActorFail);
}

void USimpleReplicationDriver::InitializeActorsInWorld(UWorld* InWorld)
{
	checkf(World == InWorld, TEXT("USimpleReplicationDriver::InitializeActorsInWorld world mismatch. %s vs %s"), *GetPathNameSafe(World), *GetPathNameSafe(InWorld));
	if (World)
	{
		for (FActorIterator Iter(World); Iter; ++Iter)
		{
			AddNetworkActor(*Iter);
		}
	}
}

void USimpleReplicationDriver::AddClientConnection(UNetConnection* NetConnection)
{
	if (NetConnection->GetUChildConnection() != nullptr)
	{
		UE_LOG(LogSimpleRepDriver, Warning, TEXT("USimpleReplicationDriver::AddClientConnection was called with a child connection, dropping."));
		return;
	}
	USimpleReplicationConnectionDriver* NetConnectionDriver = NewObject<USimpleReplicationConnectionDriver>(this);
	NetConnectionDriver->NetConnection = NetConnection;
	NetConnectionDriver->ReplicationDriver = this;
	NetConnection->SetReplicationConnectionDriver(NetConnectionDriver);
	ConnectionDrives.Add(NetConnectionDriver);
}

void USimpleReplicationDriver::RemoveClientConnection(UNetConnection* NetConnection)
{
	if (NetConnection->GetUChildConnection() != nullptr)
	{
		UE_LOG(LogSimpleRepDriver, Warning, TEXT("USimpleReplicationDriver::RemoveClientConnection was called with a child connection!"));
		return;
	}
	for (int32 Index = 0; Index < ConnectionDrives.Num(); ++Index)
	{
		const USimpleReplicationConnectionDriver* ConnectionDriver = ConnectionDrives[Index];
		if (ConnectionDriver->NetConnection == NetConnection)
		{
			if(FGrid** Grid = NetConnection2Grid.Find(NetConnection))
			{
				if((*Grid)->RemoveNetConnection(NetConnection))
				{
					RemoveGridFromQuadTree(*Grid);
				}
				NetConnection2Grid.Remove(NetConnection);
			}
			ConnectionDrives.RemoveAtSwap(Index, 1, false);
			if(USimpleReplicationConnectionDriver* SimpleReplicationConnectionDriver = Cast<USimpleReplicationConnectionDriver>(NetConnection->GetReplicationConnectionDriver()))
			{
				SimpleReplicationConnectionDriver->CleanUp();
			}
			break;
		}
	}
	if(ForceUpdateConnectionDrivers.Num())
	{
		for(TSet<USimpleReplicationConnectionDriver*>::TIterator Iterator = ForceUpdateConnectionDrivers.CreateIterator();Iterator;++Iterator)
		{
			const USimpleReplicationConnectionDriver* ReplicationConnectionDriver = *Iterator;
			if(ReplicationConnectionDriver->NetConnection == NetConnection)
			{
				Iterator.RemoveCurrent();
				break;
			}
		}
	}
	if(ForceCloseConnectionDrivers.Num())
	{
		for(TSet<USimpleReplicationConnectionDriver*>::TIterator Iterator = ForceCloseConnectionDrivers.CreateIterator();Iterator;++Iterator)
		{
			const USimpleReplicationConnectionDriver* ReplicationConnectionDriver = *Iterator;
			if(ReplicationConnectionDriver->NetConnection == NetConnection)
			{
				Iterator.RemoveCurrent();
				break;
			}
		}
	}	
}

void USimpleReplicationDriver::AddNetworkActor(AActor* Actor)
{
	if(Actor->IsA(AWorldSettings::StaticClass()) || Actor->IsNetStartupActor())
	{
		return;
	}
	if(ShouldReplicateActor(Actor))
	{
		if(Actor->bAlwaysRelevant)
		{
			AlwaysRelevantActors.Emplace(Actor);
			DirtyAlwaysRelevantActors.Emplace(Actor);
		}else if(Actor->bOnlyRelevantToOwner)
		{
			ActorsWithoutNetConnection.Emplace(Actor);
		}else if(!Actor->GetIsCanMarkDirty())
		{
			NormalActors.Emplace(Actor);
			UE_LOG(LogSimpleRepDriver, Warning, TEXT("USimpleReplicationDriver::AddNetworkActor: %s Is Not Support Mark Dirty"), *GetNameSafe(Actor));
		}else
		{
			DirtyActors.Emplace(Actor);
		}
	}
}

void USimpleReplicationDriver::RemoveNetworkActor(AActor* Actor)
{
	if(Actor->bAlwaysRelevant)
	{
		AlwaysRelevantActors.Remove(Actor);
		DirtyAlwaysRelevantActors.Remove(Actor);
	}else if(Actor->bOnlyRelevantToOwner)
	{
		ActorsWithoutNetConnection.Remove(Actor);
	}else if(!Actor->GetIsCanMarkDirty())
	{
		NormalActors.Remove(Actor);
	}else
	{
		DirtyActors.Remove(Actor);
		if(FGrid** Grid = Actor2Grid.Find(Actor))
		{
			(*Grid)->RemoveNetworkActor(Actor,GetCullDistanceLevel(Actor));
		}
		RemoveTinderActor(Actor);
	}
	NotifyActor2Number.Remove(Actor);
}

void USimpleReplicationDriver::ForceNetUpdate(AActor* Actor)
{
	MarkIsDirty(Actor);
}

void USimpleReplicationDriver::NotifyActorTearOff(AActor* Actor)
{
	RemoveNetworkActor(Actor);
}

int32 USimpleReplicationDriver::GetCullDistanceLevel(const AActor* Actor) const
{
	const float NetCullDistanceSquared = Actor->NetCullDistanceSquared;
	if(NetCullDistanceSquared <= NearNetCullDistanceSquared)
	{
		return NEAR_DISTANCE;
	}else if(NetCullDistanceSquared <= MiddleNetCullDistanceSquared)
	{
		return MIDDLE_DISTANCE;
	}
	else
	{
		return FAR_DISTANCE;
	}
}

void USimpleReplicationDriver::ForceUpdateConnectionDriver(USimpleReplicationConnectionDriver* ConnectionDriver)
{
	ForceUpdateConnectionDrivers.Emplace(ConnectionDriver);
}

void USimpleReplicationDriver::ForceCloseConnectionDriver(UNetConnection* NetConnection)
{
	if(USimpleReplicationConnectionDriver* SimpleReplicationConnectionDriver = Cast<USimpleReplicationConnectionDriver>(NetConnection->GetReplicationConnectionDriver()))
	{
		ForceCloseConnectionDrivers.Emplace(SimpleReplicationConnectionDriver);
	}
}

void USimpleReplicationDriver::AddToAlwaysRelevant(AActor* Actor, APlayerController* PlayerController)
{
	if(Actor2AlwaysControllers.Contains(Actor))
	{
		TSet<APlayerController*> Controllers = Actor2AlwaysControllers[Actor];
		Controllers.Add(PlayerController);
	}
	else
	{
		TSet<APlayerController*> Controllers;
		Controllers.Add(PlayerController);
		Actor2AlwaysControllers.Add(Actor,Controllers);
	}
	if(PlayerController->NetConnection)
	{
		ReplicateSingleActor(Actor,PlayerController->NetConnection,true,true);
	}
}

void USimpleReplicationDriver::RemoveFromAlwaysRelevant(AActor* Actor, APlayerController* PlayerController)
{
	if(Actor2AlwaysControllers.Contains(Actor))
	{
		TSet<APlayerController*> Controllers = Actor2AlwaysControllers[Actor];
		Controllers.Remove(PlayerController);
		if(Controllers.Num() == 0)
		{
			Actor2AlwaysControllers.Remove(Actor);
		}		
	}
}

void USimpleReplicationDriver::AddToNormalActors(AActor* Actor)
{
	if (Actor)
	{
		NormalActors.Emplace(Actor);
	}
}

void USimpleReplicationDriver::RemoveFromNormalActors(AActor* Actor)
{
	if (Actor)
	{
		NormalActors.Remove(Actor);
		DirtyActors.Emplace(Actor);
	}
}

void USimpleReplicationDriver::MarkIsDirty(AActor* Actor)
{
	if(ShouldReplicateActor(Actor))
	{
		if(Actor->bAlwaysRelevant)
		{
			DirtyAlwaysRelevantActors.Emplace(Actor);
		}else if(Actor->bOnlyRelevantToOwner)
		{
			ActorsWithoutNetConnection.Emplace(Actor);
		}else if(Actor->GetIsCanMarkDirty())
		{
			DirtyActors.Emplace(Actor);
		}
		TArray<AActor*> ChildActors;
		Actor->GetAttachedActors(ChildActors,false);
		for(AActor* ChildActor : ChildActors)
		{
			MarkIsDirty(ChildActor);
		}
	}	
}

uint16 USimpleReplicationDriver::GetGridIndex(const double X) const
{
	return FMath::Clamp(FMath::CeilToInt((X + HalfMapSize) / CellSize),0, MaxCellIndex);
}

FGrid* USimpleReplicationDriver::GetGridByLocation(FVector Location)
{
	return GetGridByIndex(GetGridIndex(Location.X),GetGridIndex(Location.Y));
}

FGrid* USimpleReplicationDriver::GetGridByIndex(const uint16 X, const uint16 Y)
{
	return Grids[X][Y];
}

void USimpleReplicationDriver::GetCanSeeConnections(const AActor* Actor, TSet<UNetConnection*>& Connections)
{
	const FVector Location = Actor->GetActorLocation();
	const uint16 X = GetGridIndex(Location.X);
	const uint16 Y = GetGridIndex(Location.Y);
	const uint16 Diff = GridDiffs[GetCullDistanceLevel(Actor)];
	const uint16 StartX = FMath::Max(X - Diff,0);
	const uint16 EndX = FMath::Min(X + Diff,MaxCellIndex);
	const uint16 StartY = FMath::Max(Y - Diff,0);
	const uint16 EndY = FMath::Min(Y + Diff,MaxCellIndex);
	
	const float MinX = StartX * CellSize - HalfMapSize;
	const float MinY = StartY * CellSize - HalfMapSize;

	const float MaxX = (EndX + 1) * CellSize - HalfMapSize;
	const float MaxY = (EndY + 1) * CellSize - HalfMapSize;
	const FBox2D Box2d(FVector2D(MinX,MinY),FVector2D(MaxX,MaxY));
	TArray<FGrid*> CanSeeGrids;
	QuadTree.GetElements(Box2d,CanSeeGrids);
	for(FGrid* Grid : CanSeeGrids)
	{
		for(TWeakObjectPtr<UNetConnection> NetConnectionPtr : Grid->GetNetConnections())
		{
			if(NetConnectionPtr.IsValid())
			{
				Connections.Emplace(NetConnectionPtr.Get());
			}
		}
	}
}

void USimpleReplicationDriver::OnObjectNetPropertyChanged(const UObject* const Object)
{
	if(IsValid(Object) && Object->GetWorld() == World)
	{
		const AActor* Actor = Cast<AActor>(Object);
		if(Actor == nullptr)
		{
			Actor = Object->GetTypedOuter<AActor>();
		}
		if(Actor)
		{
			MarkIsDirty((AActor*)Actor);
		}
	}
}

void USimpleReplicationDriver::OnGameThreadWakeup()
{
	if (NetDriver)
	{
		++NetDriver->ReplicationFrame;
		
		NetDriver->Dispatch();
		if (DirtyActors.Num())
		{
			ReplicateDirtyActors(DirtyActors);
			DirtyActors.Reset();
		}

		FlushPendingConnections();
	}
}

void USimpleReplicationDriver::OnConnectionMoved(UNetConnection* NetConnection, FGrid* OldGrid, FGrid* NewGrid)
{
	NetConnection2Grid.Add(NetConnection,NewGrid);
	if(!OldGrid)
	{
		if(NetConnection->PlayerController)
		{
			ReplicateSingleActor(NetConnection->PlayerController,NetConnection,true,true);
			for(TMap<AActor*,TSet<APlayerController*>>::TIterator Iterator= Actor2AlwaysControllers.CreateIterator();Iterator;++Iterator)
			{
				AActor* Actor = Iterator.Key();
				TSet<APlayerController*> Controllers = Iterator.Value();
				if(IsValid(Actor) && Controllers.Contains(NetConnection->PlayerController))
				{
					ReplicateSingleActor(Actor,NetConnection,true,true);
				}
			}
		}
		for(TWeakObjectPtr<AActor> ActorPtr : AlwaysRelevantActors)
		{
			if(ActorPtr.IsValid())
			{
				ReplicateSingleActor(ActorPtr,NetConnection,true,true);
			}
		}
	}
	TArray<FGrid*> NewInSightGrids;
	TArray<FGrid*> NewOutSightGrids;
	const uint16 MyDistanceLevel = NetConnection->ViewTarget? GetCullDistanceLevel(NetConnection->ViewTarget):-1;
	for(uint16 DistanceLevel = 0;DistanceLevel <= MAX_DISTANCE;++DistanceLevel)
	{
		const uint16 Diff = GridDiffs[DistanceLevel];
		GetChangedGrids(OldGrid,NewGrid,Diff,NewInSightGrids,NewOutSightGrids);
		for(FGrid* Grid : NewInSightGrids)
		{
			ReplicateActorsForConnection(NetConnection,Grid,DistanceLevel);
		}
		for(FGrid* Grid : NewOutSightGrids)
		{
			CloseActorChannelsForConnection(NetConnection,Grid,DistanceLevel);
		}
		if(MyDistanceLevel == DistanceLevel)
		{
			for(FGrid* Grid : NewOutSightGrids)
			{
				for(TWeakObjectPtr<UNetConnection> ConnectionPtr : Grid->GetNetConnections())
				{
					if(ConnectionPtr.IsValid())
					{
						CloseActorChannel(ConnectionPtr.Get(),NetConnection->ViewTarget);
					}
				}
			}
		}
	}
}

void USimpleReplicationDriver::OnActorMoved(AActor* Actor, FGrid* OldGrid, FGrid* NewGrid)
{
	const uint16 Diff = GridDiffs[GetCullDistanceLevel(Actor)];
	TArray<FGrid*> NewInSightGrids;
	TArray<FGrid*> NewOutSightGrids;
	GetChangedGrids(OldGrid,NewGrid,Diff,NewInSightGrids,NewOutSightGrids);
	if(TinderActor2Grid.Contains(Actor))
	{
		RemoveTinderActor(Actor);
		if(NewGrid->AddTinderActor(Actor))
		{
			AddGridToTinderQuadTree(NewGrid);
		}
		TinderActor2Grid.Add(Actor,NewGrid);
	}
	for(FGrid* Grid : NewOutSightGrids)
	{
		for(TWeakObjectPtr<UNetConnection> NetConnection : Grid->GetNetConnections())
		{
			if(NetConnection.IsValid() && !IsActorRelevantForConnection(Actor,NetConnection.Get(), false))
			{
				CloseActorChannel(NetConnection.Get(),Actor);
			}
		}
	}
}

void USimpleReplicationDriver::AddGridToQuadTree(FGrid* Grid)
{
	QuadTree.Insert(Grid,GetGridBox(Grid));
}

void USimpleReplicationDriver::RemoveGridFromQuadTree(FGrid* Grid)
{
	QuadTree.Remove(Grid,GetGridBox(Grid));
}

void USimpleReplicationDriver::AddGridToTinderQuadTree(FGrid* Grid)
{
	TinderQuadTree.Insert(Grid,GetGridBox(Grid));
}

void USimpleReplicationDriver::RemoveGridFromTinderQuadTree(FGrid* Grid)
{
	TinderQuadTree.Remove(Grid,GetGridBox(Grid));
}

void USimpleReplicationDriver::FlushPendingConnections()
{
	if(PendingFlushNetConnections.Num())
	{
		for(TWeakObjectPtr<UNetConnection> ConnectionPtr : PendingFlushNetConnections)
		{
			if(ConnectionPtr.IsValid() && ConnectionPtr->GetDriver())
			{
				ConnectionPtr->FlushNet();
			}
		}
		PendingFlushNetConnections.Reset();
	}
}

FBox2D USimpleReplicationDriver::GetGridBox(const FGrid* Grid) const
{
	const float CenterX = (Grid->X + 0.5f) * CellSize - HalfMapSize;
	const float CenterY = (Grid->Y + 0.5f) * CellSize - HalfMapSize;
	return FBox2D(FVector2D(CenterX - 100, CenterY - 100), FVector2D(CenterX + 100, CenterY + 100));
}

void USimpleReplicationDriver::OnAddActorChannel(AActor* Actor)
{
	if(ULuaManger::Get()->IsRegisteredRelevantActor(Actor))
	{
		if(NotifyActor2Number.Contains(Actor))
		{				
			const int32 OldValue = NotifyActor2Number[Actor];
			NotifyActor2Number.Emplace(Actor,OldValue + 1);
		}
		else
		{
			NotifyActor2Number.Emplace(Actor,1);
			ULuaManger::Get()->OnActorRelevantChange(Actor,true);
		}
	}			
}

void USimpleReplicationDriver::OnRemoveActorChannel(AActor* Actor)
{
	if(ULuaManger::Get()->IsRegisteredRelevantActor(Actor))
	{
		if(NotifyActor2Number.Contains(Actor))
		{
			const int32 OldValue = NotifyActor2Number[Actor];
			if(OldValue == 1)
			{
				NotifyActor2Number.Remove(Actor);
				ULuaManger::Get()->OnActorRelevantChange(Actor,false);
			}
			else
			{
				NotifyActor2Number.Emplace(Actor,OldValue - 1);
			}		
		}
	}	
}

void USimpleReplicationDriver::OnReplicateActorFail(APlayerController* PlayerController, AActor* Actor)
{
	if(PlayerController->NetConnection && IsActorRelevantForConnection(Actor,PlayerController->NetConnection, false,true))
	{
		ReplicateSingleActor(Actor,PlayerController->NetConnection,true,true);
	}
}

bool USimpleReplicationDriver::ProcessRemoteFunction(AActor* Actor, UFunction* Function, void* Parameters,
                                                     FOutParmRec* OutParams, FFrame* Stack, UObject* SubObject)
{
	if (IsActorValidForReplication(Actor) == false || Actor->IsActorBeingDestroyed())
	{
		UE_LOG(LogSimpleRepDriver, Warning, TEXT("USimpleReplicationDriver::ProcessRemoteFunction: Actor %s destroyed or not ready! Function: %s."), *GetNameSafe(Actor), *GetNameSafe(Function));
		return true;
	}

	// get the top most function
	while( Function->GetSuperFunction() )
	{
		Function = Function->GetSuperFunction();
	}

	// If we have a subobject, thats who we are actually calling this on. If no subobject, we are calling on the actor.
	UObject* TargetObj = SubObject ? SubObject : Actor;

	// Make sure this function exists for both parties.
	const FClassNetCache* ClassCache = NetDriver->NetCache->GetClassNetCache( TargetObj->GetClass() );
	if (!ClassCache)
	{
		UE_LOG(LogSimpleRepDriver, Warning, TEXT("ClassNetCache empty, not calling %s::%s"), *Actor->GetName(), *Function->GetName());
		return true;
	}
		
	const FFieldNetCache* FieldCache = ClassCache->GetFromField( Function );
	if ( !FieldCache )
	{
		UE_LOG(LogSimpleRepDriver, Warning, TEXT("FieldCache empty, not calling %s::%s TargetObj->GetClass() : %s"), *Actor->GetName(), *Function->GetName(), *TargetObj->GetClass()->GetName());
		return true;
	}
	const bool bIsServer = NetDriver->IsServer();
	if (bIsServer && (Function->FunctionFlags & FUNC_NetMulticast))
	{
		// EProcessRemoteFunctionFlags RemoteFunctionFlags = EProcessRemoteFunctionFlags::None;
		TSharedPtr<FRepLayout> RepLayout = NetDriver->GetFunctionRepLayout( Function );
		TSet<UNetConnection*> Connections;
		if(Actor->bAlwaysRelevant)
		{
			for (int32 Index = 0; Index < ConnectionDrives.Num(); ++Index)
			{
				const USimpleReplicationConnectionDriver* ConnectionDriver = ConnectionDrives[Index];
				if (ConnectionDriver->NetConnection)
				{
					Connections.Add(ConnectionDriver->NetConnection);
				}
			}
		}
		else
		{
			GetCanSeeConnections(Actor,Connections);
		}
		for (UNetConnection* Connection : Connections)
		{
			if(Connection->ViewTarget)
			{
				if (Connection->GetUChildConnection() != nullptr)
				{
					Connection = ((UChildConnection*)Connection)->Parent;
				}
				if(IsActorRelevantForConnection(Actor,Connection, true))
				{
					UActorChannel* Channel =  Connection->FindActorChannelRef(Actor);
					if (!Channel)
					{
						Channel = (UActorChannel*)Connection->CreateChannelByName( NAME_Actor, EChannelCreateFlags::OpenedLocally );
						if(Channel)
						{
							Channel->SetChannelActor(Actor, ESetChannelActorFlags::None);
						}
					}
					if (Channel)
					{
						RepLayout->BuildSharedSerializationForRPC(Parameters);
						NetDriver->ProcessRemoteFunctionForChannel(Channel, ClassCache, FieldCache, TargetObj, Connection, Function, Parameters, OutParams, Stack, true, UNetDriver::ForceSend);
						PendingFlushNetConnections.Emplace(Connection);
					}
				}
			}
		}
		RepLayout->ClearSharedSerializationForRPC();
		return true;
	}
	
	UNetConnection* Connection = Actor->GetNetConnection();
	if (Connection)
	{
		if (Connection->GetUChildConnection())
		{
			Connection = ((UChildConnection*)Connection)->Parent;
		}
	
		if (Connection->State == USOCK_Closed)
		{
			return true;
		}

		UActorChannel* Ch = Connection->FindActorChannelRef(Actor);
		if (Ch == nullptr)
		{
			if (Actor->IsPendingKillPending() || !NetDriver->IsLevelInitializedForActor(Actor, Connection))
			{
				return true;
			}
			Ch = (UActorChannel *)Connection->CreateChannelByName( NAME_Actor, EChannelCreateFlags::OpenedLocally );
			Ch->SetChannelActor(Actor, ESetChannelActorFlags::None);
		}
		NetDriver->ProcessRemoteFunctionForChannel(Ch, ClassCache, FieldCache, TargetObj, Connection, Function, Parameters, OutParams, Stack, true,UNetDriver::ForceSend);
		PendingFlushNetConnections.Emplace(Connection);
	}
	else
	{
		UE_LOG(LogSimpleRepDriver, Warning, TEXT("USimpleReplicationDriver::ProcessRemoteFunction: No owning connection for actor %s. Function %s will not be processed."), *Actor->GetName(), *Function->GetName());
	}
	return true;
}

int32 USimpleReplicationDriver::ServerReplicateActors(float DeltaSeconds)
{
	check( World );
	int32 Updated = 0;
	++NetDriver->ReplicationFrame;
	QUICK_SCOPE_CYCLE_COUNTER(USimpleReplicationDriver_ReplicateActors)
	TMap<UNetConnection*,FGrid*> MovedConnections;
	for ( int32 ConnIdx = 0; ConnIdx < ConnectionDrives.Num(); ConnIdx++ )
	{
		USimpleReplicationConnectionDriver* ConnectionDriver = ConnectionDrives[ConnIdx];
		if(UNetConnection* Connection = ConnectionDriver->NetConnection)
		{
			check( Connection );
			check( Connection->State ==USOCK_Pending || Connection->State ==USOCK_Open || Connection->State ==USOCK_Closed );
			checkSlow( Connection->GetUChildConnection() == NULL );
			AActor* OwningActor = Connection->OwningActor;
			if (OwningActor != NULL && Connection->State == USOCK_Open)
			{
				check( World == OwningActor->GetWorld() );
				AActor* DesiredViewTarget = NULL;
				if (Connection->PlayerController)
				{
					if (AActor* ViewTarget = Connection->PlayerController->GetViewTarget())
					{
						if (ViewTarget->GetWorld() && ViewTarget != Connection->PlayerController)
						{
							DesiredViewTarget = ViewTarget;
						}						
					}
				}
				Connection->ViewTarget = DesiredViewTarget;
			}
			if(Connection->ViewTarget)
			{
				FGrid* NewGrid = GetGridByLocation(Connection->ViewTarget->GetActorLocation());
				FGrid** OldGrid = NetConnection2Grid.Find(Connection);
				if(OldGrid == nullptr || (*OldGrid) != NewGrid)
				{
					if(OldGrid)
					{
						if((*OldGrid)->RemoveNetConnection(Connection))
						{
							RemoveGridFromQuadTree(*OldGrid);
						}
					}
					if(NewGrid->AddNetConnection(Connection))
					{
						AddGridToQuadTree(NewGrid);
					}					
					MovedConnections.Emplace(Connection,NewGrid);
				}
			}
		}
	}
	
	if(ForceUpdateConnectionDrivers.Num() > 0)
	{
		for(TSet<USimpleReplicationConnectionDriver*>::TIterator Iterator = ForceUpdateConnectionDrivers.CreateIterator();Iterator;++Iterator)
		{
			USimpleReplicationConnectionDriver* ConnectionDriver= *Iterator;
			if(ConnectionDriver->ReplicateAllPendingActors())
			{
				Iterator.RemoveCurrent();
			}
		}
	}	

	if(DirtyActors.Num())
	{
		 ReplicateDirtyActors(DirtyActors);
	}
	
	if(NormalActors.Num())
	{
		ReplicateDirtyActors(NormalActors);
	}

	DirtyActors.Reset();
	
    UpdateMovedConnections(MovedConnections);

	if(ConnectionDrives.Num())
	{
		for(USimpleReplicationConnectionDriver* SimpleReplicationConnectionDriver : ConnectionDrives)
		{
			if (SimpleReplicationConnectionDriver->PrepareForReplication() == false)
			{
				continue;
			}

			FNetViewerArray ConnectionViewers;
			UNetConnection* const NetConnection = SimpleReplicationConnectionDriver->NetConnection;
			APlayerController* const PC = NetConnection->PlayerController;
			const bool bReplayConnection = NetConnection->IsReplay();
			if (bReplayConnection && !NetConnection->IsReplayReady())
			{
				continue;
			}
			ConnectionViewers.Emplace(NetConnection, 0.f);
			// Send ClientAdjustments (movement RPCs) do this first and never let bandwidth saturation suppress these.
			if (PC)
			{
				PC->SendClientAdjustment();
			}
		}

		for(TWeakObjectPtr<AActor> ActorPtr : AlwaysRelevantActors)
		{
			for(USimpleReplicationConnectionDriver* SimpleReplicationConnectionDriver : ConnectionDrives)
			{
				ReplicateSingleActor(ActorPtr,SimpleReplicationConnectionDriver->NetConnection,true,true);
			}
		}
		
		for(TWeakObjectPtr<AActor> ActorPtr : DirtyAlwaysRelevantActors)
		{
			for(USimpleReplicationConnectionDriver* SimpleReplicationConnectionDriver : ConnectionDrives)
			{
				ReplicateSingleActor(ActorPtr,SimpleReplicationConnectionDriver->NetConnection,true,true);
			}
		}
	}
	DirtyAlwaysRelevantActors.Reset();
	
	if(ActorsWithoutNetConnection.Num())
	{
		for(TSet<TWeakObjectPtr<AActor>>::TIterator Iterator = ActorsWithoutNetConnection.CreateIterator();Iterator;++Iterator)
		{
			const TWeakObjectPtr<AActor> ActorPtr = *Iterator;
			if(ActorPtr.IsValid() == false || ActorPtr->IsPendingKillPending())
			{
				Iterator.RemoveCurrent();
				continue;
			}
			if (!ActorPtr->IsActorInitialized())
			{
				continue;
			}
			if(UNetConnection* NetConnection = ActorPtr->GetNetConnection())
			{
				ReplicateSingleActor(ActorPtr,NetConnection,true,true);
				//Iterator.RemoveCurrent();
			}
		}
	}
	
	FlushPendingConnections();
	
	for(USimpleReplicationConnectionDriver* SimpleReplicationConnectionDriver : ForceCloseConnectionDrivers)
	{
		if(SimpleReplicationConnectionDriver->NetConnection)
		{
			SimpleReplicationConnectionDriver->NetConnection->Close();
		}
	}
	ForceCloseConnectionDrivers.Reset();
	return Updated;
}

bool USimpleReplicationDriver::ShouldReplicateActor(AActor* Actor) const
{
	QUICK_SCOPE_CYCLE_COUNTER(USimpleReplicationDriver_ShouldReplicateActor)
	if (Actor == nullptr || Actor->IsPendingKillPending() || Actor->IsActorBeingDestroyed() || Actor->IsUnreachable())
	{
		return false;
	}
	
	// if (!IsValidChecked(Actor))
	if (!IsValid(Actor))
		return false;

	if (Actor->GetIsReplicated() == false)
		return false;

	if (Actor->GetTearOff())
		return false;

	if (Actor->NetDormancy == DORM_Initial && Actor->IsNetStartupActor())
		return false;
	
	return IsValid(Actor) && ULevel::IsNetActor(Actor) && NetDriver->ShouldReplicateActor(Actor);
}

void USimpleReplicationDriver::DumpInfo()
{
	UE_LOG(LogSimpleRepDriver, Log, TEXT("USimpleReplicationDriver Start To Dump Info"));
	for(uint16 i = 0;i<=MaxCellIndex;++i)
	{
		for(uint16 j =0;j<=MaxCellIndex;++j)
		{
			FGrid* Grid = GetGridByIndex(i,j);
			Grid->DumpInfo();
		}
	}
	UE_LOG(LogSimpleRepDriver, Log, TEXT("USimpleReplicationDriver Finish Dump Info"));
}

void USimpleReplicationDriver::SearchPlayersAt(FVector Location, float Distance, TArray<AActor*>& Actors)
{
	const uint16 X = GetGridIndex(Location.X);
	const uint16 Y = GetGridIndex(Location.Y);
	const uint16 Diff = FMath::CeilToInt(Distance / CellSize);
	const uint16 StartX = FMath::Max(X - Diff,0);
	const uint16 EndX = FMath::Min(X + Diff,MaxCellIndex);
	const uint16 StartY = FMath::Max(Y - Diff,0);
	const uint16 EndY = FMath::Min(Y + Diff,MaxCellIndex);
	
	const float MinX = StartX * CellSize - HalfMapSize;
	const float MinY = StartY * CellSize - HalfMapSize;

	const float MaxX = (EndX + 1) * CellSize - HalfMapSize;
	const float MaxY = (EndY + 1) * CellSize - HalfMapSize;
	const FBox2D Box2d(FVector2D(MinX,MinY),FVector2D(MaxX,MaxY));
	TArray<FGrid*> TempGrids;
	QuadTree.GetElements(Box2d,TempGrids);
	for(FGrid* Grid : TempGrids)
	{
		for(TWeakObjectPtr<UNetConnection> NetConnectionPtr : Grid->GetNetConnections())
		{
			if(NetConnectionPtr.IsValid() && NetConnectionPtr->ViewTarget && FVector::Dist2D(Location,NetConnectionPtr->ViewTarget->GetActorLocation()) <= Distance)
			{ 
				Actors.Add(NetConnectionPtr->ViewTarget);
			}
		}
	}
}

void USimpleReplicationDriver::SearchTinderActorsAt(FVector Location, float Distance, TArray<AActor*>& Actors)
{
	const uint16 X = GetGridIndex(Location.X);
	const uint16 Y = GetGridIndex(Location.Y);
	const uint16 Diff = FMath::CeilToInt(Distance / CellSize);
	const uint16 StartX = FMath::Max(X - Diff,0);
	const uint16 EndX = FMath::Min(X + Diff,MaxCellIndex);
	const uint16 StartY = FMath::Max(Y - Diff,0);
	const uint16 EndY = FMath::Min(Y + Diff,MaxCellIndex);
	
	const float MinX = StartX * CellSize - HalfMapSize;
	const float MinY = StartY * CellSize - HalfMapSize;

	const float MaxX = (EndX + 1) * CellSize - HalfMapSize;
	const float MaxY = (EndY + 1) * CellSize - HalfMapSize;
	const FBox2D Box2d(FVector2D(MinX,MinY),FVector2D(MaxX,MaxY));
	TArray<FGrid*> TempGrids;
	TinderQuadTree.GetElements(Box2d,TempGrids);
	for(FGrid* Grid : TempGrids)
	{
		for(TWeakObjectPtr<AActor> TinderActorPtr : Grid->GetTinderActors())
		{
			if(TinderActorPtr.IsValid() && FVector::Dist2D(Location,TinderActorPtr->GetActorLocation()) <= Distance)
			{ 
				Actors.Add(TinderActorPtr.Get());
			}
		}
	}
}

void USimpleReplicationDriver::SearchActorsAt(FVector Location, float Distance, TArray<AActor*>& Actors,
                                              UClass* Class)
{
	const uint16 X = GetGridIndex(Location.X);
	const uint16 Y = GetGridIndex(Location.Y);
	const uint16 Diff = FMath::CeilToInt(Distance / CellSize);
	const uint16 StartX = FMath::Max(X - Diff,0);
	const uint16 EndX = FMath::Min(X + Diff,MaxCellIndex);
	const uint16 StartY = FMath::Max(Y - Diff,0);
	const uint16 EndY = FMath::Min(Y + Diff,MaxCellIndex);
	for(uint16 i = StartX;i<=EndX;++i)
	{
		for(uint16 j=StartY;j<=EndY;++j)
		{
			const FGrid* Grid = GetGridByIndex(i,j);
			for(int32 DistanceLevel=0;DistanceLevel<=MAX_DISTANCE;++DistanceLevel)
			{
				for(TWeakObjectPtr<AActor> ActorPtr : Grid->GetActors(DistanceLevel))
				{
					if(ActorPtr.IsValid() && (Class == nullptr || ActorPtr->IsA(Class)) && FVector::Dist2D(Location,ActorPtr->GetActorLocation()) <= Distance)
					{ 
						Actors.Add(ActorPtr.Get());
					}
				}
			}
		}
	}
}

void USimpleReplicationDriver::AddTinderActor(AActor* Actor)
{
	FGrid* Grid = GetGridByLocation(Actor->GetActorLocation());
	TinderActor2Grid.Add(Actor,Grid);
	if(Grid->AddTinderActor(Actor))
	{
		AddGridToTinderQuadTree(Grid);
	}
}

void USimpleReplicationDriver::RemoveTinderActor(AActor* Actor)
{
	FGrid** Grid = TinderActor2Grid.Find(Actor);
	if(Grid && (*Grid)->RemoveTinderActor(Actor))
	{
		RemoveGridFromTinderQuadTree(*Grid);
	}
}

bool USimpleReplicationDriver::ReplicateSingleActor(TWeakObjectPtr<AActor> ActorPtr, UNetConnection* Connection, bool AddActorToPending, bool AddConnectionDriverToForceUpdate)
{
	QUICK_SCOPE_CYCLE_COUNTER(USimpleReplicationDriver_ReplicateSingleActor)
	if(ActorPtr.IsValid() == false || ActorPtr->IsPendingKillPending())
	{
		return true;
	}
	if(Connection->State!= USOCK_Open || !ActorPtr->IsActorInitialized() || !NetDriver->IsLevelInitializedForActor(ActorPtr.Get(), Connection) || !Connection->PlayerController)
	{
		OnFailReplicateActor(ActorPtr,Connection,AddActorToPending,AddConnectionDriverToForceUpdate);
		return false;
	}
	UActorChannel* Channel = Connection->FindActorChannelRef(ActorPtr);
	if (!Channel)
	{
		Channel = (UActorChannel*)Connection->CreateChannelByName( NAME_Actor, EChannelCreateFlags::OpenedLocally );
		if(Channel)
		{
			Channel->SetChannelActor(ActorPtr.Get(), ESetChannelActorFlags::None);
		}
	}
	if(Channel)
	{
		ActorPtr->CallPreReplication(NetDriver);
		const int64 BitWrite = Channel->ReplicateActor();
		Connection->QueuedBits -= BitWrite;
		Channel->RelevantTime = World->GetTimeSeconds();		
		if(Connection->GetPendingCloseDueToReplicationFailure())
		{
			ForceCloseConnectionDriver(Connection);
		}
		else
		{
			PendingFlushNetConnections.Emplace(Connection);
		}
		return true;
	}
	OnFailReplicateActor(ActorPtr,Connection,AddActorToPending,AddConnectionDriverToForceUpdate);
	return  false;
}

void USimpleReplicationDriver::CloseActorChannel(UNetConnection* NetConnection, AActor* Actor,
	EChannelCloseReason Reason)
{
	if(NetConnection && NetConnection->PlayerController && Actor && Actor2AlwaysControllers.Contains(Actor))
	{
		TSet<APlayerController*>* Controllers = Actor2AlwaysControllers.Find(Actor);
		if(Controllers && Controllers->Contains(NetConnection->PlayerController))
		{
			return;
		}
	}
	if(Actor && NetConnection && !Actor->IsNetStartupActor())
	 {
	 	if(UActorChannel* ActorChannel = NetConnection->FindActorChannelRef(Actor))
	 	{
	 		ActorChannel->Close(Reason);
	 	}
	 	if(USimpleReplicationConnectionDriver* SimpleReplicationConnectionDriver = Cast<USimpleReplicationConnectionDriver>(NetConnection->GetReplicationConnectionDriver()))
	 	{
	 		SimpleReplicationConnectionDriver->RemovePendingReplicateActor(Actor);
	 	}
	 }
}

void USimpleReplicationDriver::OnFailReplicateActor(TWeakObjectPtr<AActor> Actor, UNetConnection* ConnectionDriver,
                                                    bool AddActorToPending, bool AddConnectionDriverToForceUpdate)
{
	QUICK_SCOPE_CYCLE_COUNTER(USimpleReplicationDriver_OnFailReplicateActor)
	if(USimpleReplicationConnectionDriver* ReplicationConnectionDriver = Cast<USimpleReplicationConnectionDriver>(ConnectionDriver->GetReplicationConnectionDriver()))
	{
		if(AddActorToPending)
		{
			ReplicationConnectionDriver->AddPendingReplicateActor(Actor);
		}
		if(AddConnectionDriverToForceUpdate)
		{
			ForceUpdateConnectionDrivers.Emplace(ReplicationConnectionDriver);
		}
	}
}

void USimpleReplicationDriver::ReplicateActorToNearBy(AActor* Actor)
{
	QUICK_SCOPE_CYCLE_COUNTER(USimpleReplicationDriver_GetCanSeeConnections)
	TSet<UNetConnection*> Connections;
	GetCanSeeConnections(Actor,Connections);
	for(UNetConnection* NetConnection : Connections)
	{
		if(IsActorRelevantForConnection(Actor,NetConnection, true) && NetConnection2Grid.Contains(NetConnection))
		{
			ReplicateSingleActor(Actor,NetConnection,true,true);
		}
	}
}

void USimpleReplicationDriver::CloseActorChannelsForConnection(UNetConnection* Connection, FGrid* Grid,
	const uint16 DistanceLevel)
{
	QUICK_SCOPE_CYCLE_COUNTER(USimpleReplicationDriver_CloseActorChannelsForConnection)
	TSet<TWeakObjectPtr<AActor>> Actors = Grid->GetActors(DistanceLevel);
	for(TWeakObjectPtr<AActor> ActorPtr : Actors)
	{
		if(!ActorPtr.IsValid() || IsActorRelevantForConnection(ActorPtr.Get(),Connection, false))
		{
			continue;
		}
		CloseActorChannel(Connection,ActorPtr.Get());
	}	
}

void USimpleReplicationDriver::ReplicateActorsForConnection(UNetConnection* Connection, FGrid* Grid,
	const uint16 DistanceLevel)
{
	QUICK_SCOPE_CYCLE_COUNTER(USimpleReplicationDriver_ReplicateActorsForConnection)
	TSet<TWeakObjectPtr<AActor>> Actors = Grid->GetActors(DistanceLevel);
	for(TWeakObjectPtr<AActor> ActorPtr : Actors)
	{
		if(ActorPtr.IsValid() && IsActorRelevantForConnection(ActorPtr.Get(),Connection, true))
		{
			ReplicateSingleActor(ActorPtr,Connection,true,true);
		}
	}
}

void USimpleReplicationDriver::GetChangedGrids(const FGrid* OldGrid, const FGrid* NewGrid, const uint16 Diff,
	TArray<FGrid*>& NewInSightGrids, TArray<FGrid*>& NewOutSightGrids)
{
	QUICK_SCOPE_CYCLE_COUNTER(USimpleReplicationDriver_GetChangedGrids)
	NewInSightGrids.Reset();
	NewOutSightGrids.Reset();
	if(OldGrid == nullptr)
	{
		const uint16 StartX = FMath::Max(0,NewGrid->X - Diff);
		const uint16 EndX = FMath::Min(MaxCellIndex,NewGrid->X + Diff);
		const uint16 StartY = FMath::Max(0,NewGrid->Y - Diff);
		const uint16 EndY = FMath::Min(MaxCellIndex,NewGrid->Y + Diff);
		for(uint16 i = StartX;i<=EndX;++i)
		{
			for(uint16 j=StartY;j<=EndY;++j)
			{
				FGrid* Grid = GetGridByIndex(i,j);
				NewInSightGrids.Emplace(Grid);
			}
		}
	}
	else
	{
		const uint16 OldStartX = FMath::Max(0,OldGrid->X - Diff);
		const uint16 OldEndX = FMath::Min(MaxCellIndex,OldGrid->X + Diff);
		const uint16 OldStartY = FMath::Max(0,OldGrid->Y - Diff);
		const uint16 OldEndY = FMath::Min(MaxCellIndex,OldGrid->Y + Diff);

		const uint16 NewStartX = FMath::Max(0,NewGrid->X - Diff);
		const uint16 NewEndX = FMath::Min(MaxCellIndex,NewGrid->X + Diff);
		const uint16 NewStartY = FMath::Max(0,NewGrid->Y - Diff);
		const uint16 NewEndY = FMath::Min(MaxCellIndex,NewGrid->Y + Diff);
		
		for(uint16 i = OldStartX;i<=OldEndX;++i)
		{
			for(uint16 j=OldStartY;j<=OldEndY;++j)
			{
				if(!(i >= NewStartX && i <= NewEndX && j >= NewStartY && j <= NewEndY))
				{
					FGrid* Grid = GetGridByIndex(i,j);
					NewOutSightGrids.Emplace(Grid);
				}
			}
		}
		for(uint16 i = NewStartX;i<=NewEndX;++i)
		{
			for(uint16 j=NewStartY;j<=NewEndY;++j)
			{
				if(!(i >= OldStartX && i <= OldEndX && j >= OldStartY && j <= OldEndY))
				{
					FGrid* Grid = GetGridByIndex(i,j);
                    NewInSightGrids.Emplace(Grid);
				}
			}
		}
	}
}
bool USimpleReplicationDriver::IsActorRelevantForConnection(const AActor* Actor,const UNetConnection* NetConnection, bool IsDistanceRelevant,bool CheckDistanceAgain)
{
	QUICK_SCOPE_CYCLE_COUNTER(USimpleReplicationDriver_IsActorRelevantForConnection)
	const AActor* RealViewer = NetConnection->PlayerController ? NetConnection->PlayerController : NetConnection->OwningActor;
	const AActor* ViewTarget = NetConnection->ViewTarget;
	const AActor* Owner = Actor->GetOwner();
	if(NetConnection->PlayerController && Actor2AlwaysControllers.Contains(Actor))
	{
		TSet<APlayerController*>* Controllers = Actor2AlwaysControllers.Find(Actor);
		if(Controllers && Controllers->Contains(NetConnection->PlayerController))
		{
			return true;
		}
	}
	if (Actor->bAlwaysRelevant || (ViewTarget && Actor->IsOwnedBy(ViewTarget)) || Actor->IsOwnedBy(RealViewer) || Actor == ViewTarget || ViewTarget == Actor->GetInstigator())
	{
		return true;
	}
	else if (Actor->bNetUseOwnerRelevancy && Owner)
	{
		return IsActorRelevantForConnection(Owner,NetConnection, IsDistanceRelevant,CheckDistanceAgain);
	}
	else if (Actor->bOnlyRelevantToOwner)
	{
		return Owner == NetConnection->PlayerController ||
			  (NetConnection->PlayerController && Owner == NetConnection->PlayerController->GetPawn()) ||
		      (ViewTarget && ViewTarget->IsRelevancyOwnerFor( Actor, Owner, NetConnection->OwningActor));
	}
	const USceneComponent* RootComponent = Actor->GetRootComponent();
	if (RootComponent && RootComponent->GetAttachParent() && RootComponent->GetAttachParent()->GetOwner() && (Cast<USkeletalMeshComponent>(RootComponent->GetAttachParent()) || (RootComponent->GetAttachParent()->GetOwner() == Owner)))
	{
		return IsActorRelevantForConnection(RootComponent->GetAttachParent()->GetOwner(),NetConnection, IsDistanceRelevant,CheckDistanceAgain);
	}
	else if(Actor->IsHidden() && (!RootComponent || !RootComponent->IsCollisionEnabled()))
	{
		return false;
	}
	if (!RootComponent)
	{
		UE_LOG(LogSimpleRepDriver, Warning, TEXT("Actor %s / %s has no root component in AActor::IsNetRelevantFor. (Make bAlwaysRelevant=true?)"), *GetClass()->GetName(), *GetName() );
		return false;
	}
	if (ViewTarget == nullptr)
	{
		return false;
	}	
	if(CheckDistanceAgain && !IsDistanceRelevant)
	{
		TSet<UNetConnection*> Connections;
		GetCanSeeConnections(Actor,Connections);
		for(UNetConnection* Connection : Connections)
		{
			if(Connection == NetConnection)
			{
				return true;
			}
		}
	}
	return IsDistanceRelevant;
}

void USimpleReplicationDriver::ReplicateDirtyActors(TSet<TWeakObjectPtr<AActor>>& Actors)
{
	QUICK_SCOPE_CYCLE_COUNTER(USimpleReplicationDriver_ReplicateDirtyActors)
	if(Actors.Num())
	{
		for(TSet<TWeakObjectPtr<AActor>>::TIterator Iterator = Actors.CreateIterator();Iterator;++Iterator)
		{
			const TWeakObjectPtr<AActor> ActorPtr = *Iterator;
			if(ActorPtr.IsValid() == false || ActorPtr->IsPendingKillPending())
			{
				Iterator.RemoveCurrent();
				continue;
			}
			if (!ActorPtr->IsActorInitialized())
			{
				continue;
			}
			FGrid** OldGrid = Actor2Grid.Find(ActorPtr);
			FGrid* NewGrid = GetGridByLocation(ActorPtr->GetActorLocation());
			if(OldGrid == nullptr || (*OldGrid) != NewGrid)
			{
				const int32 DistanceLevel = GetCullDistanceLevel(ActorPtr.Get());
				if(OldGrid)
				{
					(*OldGrid)->RemoveNetworkActor(ActorPtr,DistanceLevel);
				}
				NewGrid->AddNetworkActor(ActorPtr,DistanceLevel);
				OnActorMoved(ActorPtr.Get(),OldGrid == nullptr ? nullptr : *OldGrid,NewGrid);
				Actor2Grid.Add(ActorPtr,NewGrid);
			}			
			if(ConnectionDrives.Num() > 0)
			{
				ReplicateActorToNearBy(ActorPtr.Get());
			}
			if(Actor2AlwaysControllers.Contains(ActorPtr.Get()))
			{
				TSet<APlayerController*>* Controllers = Actor2AlwaysControllers.Find(ActorPtr.Get());
				if(Controllers)
				{
					for(const APlayerController* PlayerController : *Controllers)
					{
						if(IsValid(PlayerController) && PlayerController->NetConnection)
						{
							ReplicateSingleActor(ActorPtr.Get(),PlayerController->NetConnection,true,true);
						}
					}
				}
			}
		}
	}
}

void USimpleReplicationDriver::UpdateMovedConnections(TMap<UNetConnection*, FGrid*>& MovedConnections)
{
	if(MovedConnections.Num() > 0)
	{
		for(TMap<UNetConnection*,FGrid*>::TIterator Iterator = MovedConnections.CreateIterator();Iterator;++Iterator)
		{
			UNetConnection* NetConnection = (*Iterator).Key;
			FGrid** OldGrid = NetConnection2Grid.Find(NetConnection);
			FGrid* NewGrid = (*Iterator).Value;
			OnConnectionMoved(NetConnection,(OldGrid ? * OldGrid : nullptr),NewGrid);
		}
	}
}

USimpleReplicationConnectionDriver::USimpleReplicationConnectionDriver()
{
}

void USimpleReplicationConnectionDriver::NotifyActorChannelAdded(AActor* Actor, UActorChannel* Channel)
{
	ReplicationDriver->OnAddActorChannel(Actor);
}

void USimpleReplicationConnectionDriver::NotifyActorChannelRemoved(AActor* Actor)
{
	ReplicationDriver->OnRemoveActorChannel(Actor);
}

void USimpleReplicationConnectionDriver::NotifyClientVisibleLevelNamesAdd(FName LevelName, UWorld* StreamingWorld)
{
	if (StreamingWorld && StreamingWorld->PersistentLevel)
	{
		if(NetConnection && NetConnection->ViewTarget)
		{
			TArray<AActor*>& Actors = StreamingWorld->PersistentLevel->Actors;
			for (AActor* Actor : Actors)
			{
				if(ReplicationDriver && ReplicationDriver->ShouldReplicateActor(Actor) && ReplicationDriver->IsActorRelevantForConnection(Actor,NetConnection,false,true))
				{
					ReplicationDriver->ReplicateSingleActor(Actor,NetConnection,true,true);
				}
			}
		}		
	}
}

void USimpleReplicationConnectionDriver::RemovePendingReplicateActor(AActor* Actor)
{
	PendingReplicateActors.Remove(Actor);
}

bool USimpleReplicationConnectionDriver::ReplicateAllPendingActors()
{
	QUICK_SCOPE_CYCLE_COUNTER(USimpleReplicationDriver_ReplicateAllPendingActors)
	if(PendingReplicateActors.Num() > 0 && NetConnection->ViewTarget)
	{
		for(TSet<TWeakObjectPtr<AActor>>::TIterator Iterator = PendingReplicateActors.CreateIterator();Iterator;++Iterator)
		{
			TWeakObjectPtr<AActor> ActorPtr = *Iterator;
			if(ActorPtr.IsValid() == false || ReplicationDriver->IsActorRelevantForConnection(ActorPtr.Get(),NetConnection, false,true) == false)
			{
				Iterator.RemoveCurrent();
				continue;
			}
			if(ReplicationDriver->ReplicateSingleActor(ActorPtr,NetConnection,false,false))
			{
				Iterator.RemoveCurrent();
			}
		}
	}
	return PendingReplicateActors.Num() == 0;
}

void USimpleReplicationConnectionDriver::AddPendingReplicateActor(TWeakObjectPtr<AActor> Actor)
{
	PendingReplicateActors.Emplace(Actor);
}

void USimpleReplicationConnectionDriver::CleanUp()
{
	PendingReplicateActors.Empty();
}

bool USimpleReplicationConnectionDriver::PrepareForReplication()
{
	NetConnection->ViewTarget = NetConnection->PlayerController ? NetConnection->PlayerController->GetViewTarget() : NetConnection->OwningActor;

	// Set any children viewtargets
	for (int32 i = 0; i < NetConnection->Children.Num(); ++i)
	{
		UNetConnection* CurChild = NetConnection->Children[i];
		CurChild->ViewTarget = CurChild->PlayerController ? CurChild->PlayerController->GetViewTarget() : CurChild->OwningActor;
	}

	return (NetConnection->State != USOCK_Closed) && (NetConnection->ViewTarget != nullptr);
}
