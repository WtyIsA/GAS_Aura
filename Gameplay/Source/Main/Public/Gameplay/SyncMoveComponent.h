#pragma once

#include "CoreMinimal.h"
#include "Player/EStateType.h"
#include "Components/SceneComponent.h"
#include "Containers/Deque.h"
// #include "Containers/Deque.h"
#include "SyncMoveComponent.generated.h"

struct FHistoryPosInfo
{
	FVector Pos;
	double Timestamp;
	FHistoryPosInfo(const FVector loc,const double timestamp)
	{
		Pos = loc;
		Timestamp = timestamp;
	}
};

class UFootPrintComponent;
class UBaseCheatChecker;
class UCharacterMovementComponent;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAIN_API USyncMoveComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	USyncMoveComponent();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	bool bEnableCheck = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	TSubclassOf<UBaseCheatChecker> CheatCheckerClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	bool bExtrapolation = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	float PosSyncInterval = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	float NetworkShakeTolerance = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Important)
	bool bSyncRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	float RotationSyncInterval = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	bool bRotationFollowVelocity = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	bool bRotationFollowVelocityIgnoreRoll = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	bool bRotationFollowVelocityIgnorePitch = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	bool bRotationFollowVelocityIgnoreYaw = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	FVector2D PitchLimit={-180,180};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Important)
	FVector2D RollLimit={-180,180};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableSync = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableServerAutoSend = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableLog= true;

	double GetLastReciveTimestamp();
	FVector GetLastVelocity() const;
	FVector GetCurVelocity() const;
	FVector GetRootPos() const;

	UFootPrintComponent* GetFootPrintComponent() const;
	void SetVelocity(FVector velocity);
	UFUNCTION(BlueprintCallable)
	void	SendPosition();
	UFUNCTION(BlueprintCallable)
	FVector GetVelocity();
	UFUNCTION(BlueprintCallable)
	float GetClientTimestamp();
	UFUNCTION(BlueprintCallable)
	float GetClientPlatformTimestamp();

	UFUNCTION(BlueprintCallable)
	void SetCurState(EStateType state);
	
	UFUNCTION(BlueprintCallable)
	EStateType GetCurState();

	UFUNCTION(BlueprintCallable)
	void SetEnableServerAutoSend(bool enable);

	UFUNCTION(BlueprintCallable)
	void RefreshMsgIndex();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UFUNCTION(Server, Unreliable)
	void SendRotation_Server(FRotator rotator);
	UFUNCTION(Server, Reliable)
	void SendPosition_Server(FVector_NetQuantize100 pos, FVector_NetQuantize100 velocity, double timestamp,EStateType state,uint32 sendPosMsgIndex);

private:
	USceneComponent* root;
	UCharacterMovementComponent* movementComponent;
	UBaseCheatChecker* CheatChecker;
	UFootPrintComponent* FootPrintComponent;
	TDeque<FHistoryPosInfo> HistoryPosInfoDeque;
	bool bServer = false;
	bool bSimulated = false;
	bool bLocal = false;
	TArray<AActor*> IgnoreActors;
	TArray<AActor*> childActors;

	FVector lastVelocity = FVector::ZeroVector;
	FVector curVelocity = FVector::ZeroVector;

	//client 
	FTimerHandle clientSendPosTimerHandle;
	FTimerHandle clientSendRotationTimerHandle;

	FVector clientLastSendPos;
	FRotator clientLastSendRotator;
	FVector clientLastFloorNormal;
	FVector lastReceivedPos = FVector::ZeroVector;
	FVector lastFramePos = FVector::ZeroVector;
	FVector lastAcce = FVector::ZeroVector;
	FRotator lastReceivedRotation = FRotator::ZeroRotator;
	double offsetWorldTimestamp = 0;
	double offsetPlatformTimestamp = 0;
	double reciveServerTimestampWorldTime = 0;
	double reciveServerTimestampPlatformTime =0;
	UPROPERTY(ReplicatedUsing = OnRep_ServerTimestamp)
	double ServerTimestamp = 0;
	UPROPERTY(Replicated)
	uint32 SendPosMsgIndex = 0;

	UPrimitiveComponent* CollisionComponent;


	bool bReachRecivedPos = false;
	float lastSendTimeStamp = 0;
	float lerpPosSpeed = 0;
	float lerpRotatorSpeed = 0;
	uint32 clientTickCount =1;
	uint32 lastSendPosTickCount =0;

	float GetChangeVelocityAngle();
	float GetChangeFloorAngle();
	float GetGroundSlopeAngle();
	float GetChangeVelocitySize();

	bool IsLocalControl();

	FVector GetGroundSlopeDir();
	FVector GetRampVelocity();

	void ClientSendPosTimerFunc();
	void ClientSendRotationTimerFunc();
	void ClientBeginPlay();
	void ClientTick(float DeltaTime);
	void ClientLerpPos(float DeltaTime);
	void ClientLerpRotation(float DeltaTime);
	void CalculateLerpVelocity();
	void LimitRotation360(FRotator& rotator);
	void RotationFollowVelocity(float DeltaTime);
	void UpdateNetRoleType();
	void RegisterLocalTimer();
	void UnRegisterLocalTimer();
	bool IsThrowWall(FVector curPos, FVector targetPos);
	bool IsOverSpeed(FVector curPos, FVector targetPos, double clientTimestamp);
	bool IsOverSpeedByHistoryPosInfo(FVector pos);

	UFUNCTION()
	void OnRep_ServerTimestamp();

	static bool SameVector(FVector one, FVector two, float threshold);

	//server
	FVector lastSendPos = FVector::ZeroVector;
	FVector lastSendRotaion = FVector::ZeroVector;
	double lastReciveTimestamp = 0;
	UPROPERTY(Replicated)
	EStateType curState = EStateType::Ground;
	EStateType lastState = EStateType::Ground;
	EStateType curGameplayState = EStateType::Default;
	void ServerBeginPlay();
	void ServerTick(float DeltaTime);
	void SendRotation();
	bool IsTimestampOk(double timestamp);
	void AddHistoryPosInfo(double timestamp,FVector pos);

	FVector GetExtrapolationPos(FVector pos, FVector extrapolationVelocity);
	void ServerAutoSend();
	void ServerAutoPos();
	void ServerAutoRotation();
	void ServerFixPos(FVector pos,bool bSweep = true);
	UFUNCTION(NetMulticast, Reliable)
	void ServerSendPos_Multicast(FVector pos);
	UFUNCTION(NetMulticast, Unreliable)
	void ServerSendRotation_Multicast(int16 roll, int16 pitch, int16 yaw);
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void ServerSetPos_Multicast(FVector pos);
	UFUNCTION(NetMulticast, Reliable)
	void RefreshSendPosMsgIndex_Multicast();
	UFUNCTION(NetMulticast, Reliable)
	void ServerSendMsgIndex_Multicast(uint32 msgIndex);
	
};
