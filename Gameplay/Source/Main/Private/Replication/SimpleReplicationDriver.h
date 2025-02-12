#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/NetDriver.h"
#include "Engine/NetConnection.h"
#include "Engine/ReplicationDriver.h"
#include "GenericQuadTree.h"
#include "SimpleReplicationDriver.generated.h"

DECLARE_LOG_CATEGORY_EXTERN( LogSimpleRepDriver, Log, All );

class USimpleReplicationDriver;

struct FGrid
{
	FGrid(uint16 ActorArraySize, uint16 InX,uint16 InY)
	{
		ActorsArray.SetNum(ActorArraySize);
		for(int32 i=0;i<ActorArraySize;++i)
		{
			TSet<TWeakObjectPtr<AActor>> Actors;
			ActorsArray[i] = Actors;
		}
		X = InX;
		Y = InY;
	}

	void AddNetworkActor(const TWeakObjectPtr<AActor> ActorPtr,const int32 DistanceLevel)
	{
		ActorsArray[DistanceLevel].Emplace(ActorPtr);
	}

	bool AddTinderActor(const TWeakObjectPtr<AActor> ActorPtr)
	{
		const int32 OldNum = TinderActors.Num();
		TinderActors.Emplace(ActorPtr);
		return OldNum == 0;
	}

	bool RemoveTinderActor(const TWeakObjectPtr<AActor> ActorPtr)
	{
		const int32 OldNum = TinderActors.Num();
		TinderActors.Remove(ActorPtr);
		return OldNum > 0 && TinderActors.Num() == 0;
	}

	void RemoveNetworkActor(const TWeakObjectPtr<AActor> ActorPtr,const int32 DistanceLevel)
	{
		ActorsArray[DistanceLevel].Remove(ActorPtr);
	}

	bool AddNetConnection(const TWeakObjectPtr<UNetConnection> Connection)
	{
		const int32 OldNum = NetConnections.Num();
		NetConnections.Emplace(Connection);
		return OldNum == 0 && NetConnections.Num() > 0;
	}
	
	bool RemoveNetConnection(const TWeakObjectPtr<UNetConnection> Connection)
	{
		const int32 OldNum = NetConnections.Num();
		NetConnections.Remove(Connection);
		return OldNum > 0 && NetConnections.Num() == 0;
	}

	void DumpInfo();

	TSet<TWeakObjectPtr<AActor>> GetActors(const int DistanceLevel) const {return ActorsArray[DistanceLevel];}
	
	TSet<TWeakObjectPtr<UNetConnection>> GetNetConnections() { return NetConnections;}

	TSet<TWeakObjectPtr<AActor>> GetTinderActors() { return TinderActors;}

	uint16 X;
	
	uint16 Y;
	
	TArray<TSet<TWeakObjectPtr<AActor>>> ActorsArray;
	
	TSet<TWeakObjectPtr<UNetConnection>> NetConnections;

	TSet<TWeakObjectPtr<AActor>> TinderActors;
};

UCLASS(transient)
class USimpleReplicationConnectionDriver : public UReplicationConnectionDriver
{
	GENERATED_BODY()
	
public:
	
	USimpleReplicationConnectionDriver();
    
	virtual void NotifyActorChannelAdded(AActor* Actor, UActorChannel* Channel) override;

	virtual void NotifyActorChannelRemoved(AActor* Actor) override;

	virtual void NotifyActorChannelCleanedUp(UActorChannel* Channel) override{}

	virtual void NotifyAddDestructionInfo(FActorDestructionInfo* DestructInfo) override {}

	virtual void NotifyAddDormantDestructionInfo(AActor* Actor) override {}

	virtual void NotifyRemoveDestructionInfo(FActorDestructionInfo* DestructInfo) override {}

	virtual void NotifyResetDestructionInfo() override {}

	virtual void NotifyClientVisibleLevelNamesAdd(FName LevelName, UWorld* StreamingWorld) override;

	virtual void NotifyClientVisibleLevelNamesRemove(FName LevelName) override {}

	void RemovePendingReplicateActor(AActor* Actor);

	bool ReplicateAllPendingActors();

	void AddPendingReplicateActor(TWeakObjectPtr<AActor> Actor);

	UPROPERTY()
	USimpleReplicationDriver* ReplicationDriver;

	UPROPERTY()
	UNetConnection* NetConnection;

	void CleanUp();
	
	bool PrepareForReplication();
private:

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> PendingReplicateActors;
};


UCLASS(transient, config=Engine)
class USimpleReplicationDriver : public UReplicationDriver
{
	GENERATED_BODY()
public:
	USimpleReplicationDriver();
	
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRelevantChange, AActor*, bool);
	
	FOnRelevantChange OnRelevantChange;

	virtual void SetRepDriverWorld(UWorld* InWorld) override;

	virtual void InitForNetDriver(UNetDriver* InNetDriver) override;

	virtual void InitializeActorsInWorld(UWorld* InWorld) override;

	virtual void ResetGameWorldState() override {}

	virtual void AddClientConnection(UNetConnection* NetConnection) override;

	virtual void RemoveClientConnection(UNetConnection* NetConnection) override;

	
	virtual void AddNetworkActor(AActor* Actor) override;

	virtual void RemoveNetworkActor(AActor* Actor) override;

	virtual void ForceNetUpdate(AActor* Actor) override;

	virtual void FlushNetDormancy(AActor* Actor, bool WasDormInitial) override {}

	virtual void NotifyActorTearOff(AActor* Actor) override;

	virtual void NotifyActorFullyDormantForConnection(AActor* Actor, UNetConnection* Connection) override {}

	virtual void NotifyActorDormancyChange(AActor* Actor, ENetDormancy OldDormancyState) override {}
 
	// virtual void NotifyDestructionInfoCreated(AActor* Actor, FActorDestructionInfo& DestructionInfo) override {}

	// virtual void SetRoleSwapOnReplicate(AActor* Actor, bool bSwapRoles) override {}

	/** Handles an RPC. Returns true if it actually handled it. Returning false will cause the rep driver function to handle it instead */
	virtual bool ProcessRemoteFunction(class AActor* Actor, UFunction* Function, void* Parameters, FOutParmRec* OutParams, FFrame* Stack, UObject* SubObject ) override;

	/** The main function that will actually replicate actors. Called every server tick. */
	virtual int32 ServerReplicateActors(float DeltaSeconds) override;

	virtual void PostTickDispatch() override {}

	bool ShouldReplicateActor(AActor* Actor) const;

	void DumpInfo();

	void SearchPlayersAt(FVector Location,float Distance,TArray<AActor*>& Actors);

	void SearchTinderActorsAt(FVector Location,float Distance,TArray<AActor*>& Actors);

	void SearchActorsAt(FVector Location,float Distance,TArray<AActor*>& Actors, UClass* Class);

	void AddTinderActor(AActor* Actor);

	void RemoveTinderActor(AActor* Actor);

	UPROPERTY()
	UNetDriver* NetDriver;

	UPROPERTY()
	TArray<USimpleReplicationConnectionDriver*> ConnectionDrives;
	
	UPROPERTY()
	UWorld* World = nullptr;

	UPROPERTY(Config)
	float CellSize;

	UPROPERTY(Config)
	float MapSize;

	UPROPERTY(Config)
	float RelevantCheckTime;

	UPROPERTY(Config)
	float NearNetCullDistance;

	UPROPERTY(Config)
	float MiddleCullDistance;

	UPROPERTY(Config)
	float FarCullDistance;	
	
	int32 GetCullDistanceLevel(const AActor* Actor) const;

	void ForceUpdateConnectionDriver(USimpleReplicationConnectionDriver* ConnectionDriver);

	void ForceCloseConnectionDriver(UNetConnection* NetConnection);

	bool ReplicateSingleActor(TWeakObjectPtr<AActor>, UNetConnection* ConnectionDriver, bool AddActorToPending, bool AddConnectionDriverToForceUpdate);

	void CloseActorChannel(UNetConnection* NetConnection,AActor* Actor,EChannelCloseReason Reason = EChannelCloseReason::Relevancy);

	bool IsActorRelevantForConnection(const AActor* Actor,const UNetConnection* NetConnection, bool IsDistanceRelevant,bool CheckDistanceAgain = false);

	void ReplicateDirtyActors(TSet<TWeakObjectPtr<AActor>>& Actors);
	
	void UpdateMovedConnections(TMap<UNetConnection*,FGrid*>& MovedConnections);

	void AddPendingFlushConnection(UNetConnection* NetConnection) { PendingFlushNetConnections.Emplace(NetConnection);}
	
	void AddNeedNotifyClass(UClass* Class);

	void AddToAlwaysRelevant(AActor* Actor, APlayerController* PlayerController);

	void RemoveFromAlwaysRelevant(AActor* Actor, APlayerController* PlayerController);

	void AddToNormalActors(AActor* Actor);
	
	void RemoveFromNormalActors(AActor* Actor);

protected:

	void MarkIsDirty(AActor* Actor);

	uint16 GetGridIndex(const double X) const;

	FGrid* GetGridByLocation(FVector Location);

	FGrid* GetGridByIndex(const uint16 X,const uint16 Y);

	void OnFailReplicateActor(TWeakObjectPtr<AActor>, UNetConnection* ConnectionDriver, bool AddActorToPending, bool AddConnectionDriverToForceUpdate);

	void ReplicateActorToNearBy(AActor* Actor);
	
	void CloseActorChannelsForConnection(UNetConnection* Connection,FGrid* Grid,const uint16 DistanceLevel);

	void ReplicateActorsForConnection(UNetConnection* Connection,FGrid* Grid,const uint16 DistanceLevel);

	void GetChangedGrids(const FGrid* OldGrid,const FGrid* NewGrid,const uint16 Diff,TArray<FGrid*>& NewInSightGrids,TArray<FGrid*>& NewOutSightGrids);
	
	TArray< TArray<FGrid*> > Grids;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> DirtyActors;

	TMap<TWeakObjectPtr<AActor>,FGrid*> Actor2Grid;

	TMap<TWeakObjectPtr<UNetConnection>,FGrid*> NetConnection2Grid;

	UPROPERTY()
	TArray<uint16> GridDiffs;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> AlwaysRelevantActors;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> DirtyAlwaysRelevantActors;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> ActorsWithoutNetConnection;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> NormalActors;
	
	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> CanSeeActors;

	UPROPERTY()
	TSet<USimpleReplicationConnectionDriver*> ForceUpdateConnectionDrivers;

	UPROPERTY()
	TSet<USimpleReplicationConnectionDriver*> ForceCloseConnectionDrivers;

	UPROPERTY()
	TSet<TWeakObjectPtr<UNetConnection>> PendingFlushNetConnections;

	void GetCanSeeConnections(const AActor* Actor,TSet<UNetConnection*>& Connections);

private:
	
	void OnObjectNetPropertyChanged(const UObject* const Object);

	void OnGameThreadWakeup();

	void OnConnectionMoved(UNetConnection* NetConnection,FGrid* OldGrid,FGrid* NewGrid);

	void OnActorMoved(AActor* Actor,FGrid* OldGrid,FGrid* NewGrid);

	void AddGridToQuadTree(FGrid* Grid);

	void RemoveGridFromQuadTree(FGrid* Grid);
	
	void AddGridToTinderQuadTree(FGrid* Grid);

	void RemoveGridFromTinderQuadTree(FGrid* Grid);
	
	void FlushPendingConnections();

	FBox2D GetGridBox(const FGrid* Grid) const;

	void OnAddActorChannel(AActor* Actor);

	void OnRemoveActorChannel(AActor* Actor);

	int32 MaxCellIndex;
	float HalfMapSize;
	float NearNetCullDistanceSquared;
	float MiddleNetCullDistanceSquared;
	float FarNetCullDistanceSquared;

	TQuadTree<FGrid*, 20> QuadTree;

	TQuadTree<FGrid*, 20> TinderQuadTree;

	TMap<AActor*,FGrid*> TinderActor2Grid;	

	UPROPERTY()
	TMap<AActor*,int32> NotifyActor2Number;

	friend class USimpleReplicationConnectionDriver;

	void OnReplicateActorFail(APlayerController* PlayerController,AActor* Actor);

	TMap<AActor*,TSet<APlayerController*>> Actor2AlwaysControllers;
};