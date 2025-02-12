#include "Gameplay/SyncMoveComponent.h"

#include "BaseCheatChecker.h"
#include "DrawDebugHelpers.h"
#include "FootPrintComponent.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/CoreDelegates.h"
#include "Net/UnrealNetwork.h"

USyncMoveComponent::USyncMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USyncMoveComponent::BeginPlay()
{
	Super::BeginPlay();
	root = GetOwner()->GetRootComponent();
	UActorComponent* comp = GetOwner()->GetComponentByClass(UBaseCheatChecker::StaticClass());
	if (comp)
	{
		CheatChecker = Cast<UBaseCheatChecker>(comp);
		CheatChecker->SyncMoveComponent = this;
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("NO CheatChecker:%s"),*GetOwner()->GetName());
	}
	UActorComponent* foot = GetOwner()->GetComponentByClass(UFootPrintComponent::StaticClass());
	if (foot)
	{
		FootPrintComponent = Cast<UFootPrintComponent>(foot);
		FootPrintComponent->AddFootPrint(root->GetComponentLocation(),FPlatformTime::Seconds(),0);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("NO UFootPrintComponent:%s"),*GetOwner()->GetName());
	}
	SetIsReplicated(true);
	IgnoreActors.Add(GetOwner());
	bServer = GetWorld()->IsServer();
	UpdateNetRoleType();
	bServer ? ServerBeginPlay() : ClientBeginPlay();
}

void USyncMoveComponent::ClientBeginPlay()
{
	movementComponent = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
}

void USyncMoveComponent::ServerBeginPlay()
{
	if (CheatChecker != nullptr)
	{
		CheatChecker->SyncMoveComponent = this;
	}
	ServerTimestamp = FPlatformTime::Seconds();
	lastReciveTimestamp = FPlatformTime::Seconds();
	AddHistoryPosInfo(ServerTimestamp,GetRootPos());
}

void USyncMoveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UnRegisterLocalTimer();
}

double USyncMoveComponent::GetLastReciveTimestamp()
{
	return lastReciveTimestamp;
}

FVector USyncMoveComponent::GetLastVelocity() const
{
	return lastVelocity;
}

FVector USyncMoveComponent::GetCurVelocity() const
{
	return curVelocity;
}

FVector USyncMoveComponent::GetRootPos() const
{
	return root->GetComponentLocation();
}

UFootPrintComponent* USyncMoveComponent::GetFootPrintComponent() const
{
	if (FootPrintComponent)
	{
		return FootPrintComponent;
	}
	return nullptr;
}

void USyncMoveComponent::SetVelocity(FVector velocity)
{
	curVelocity = velocity;
}

void USyncMoveComponent::SendPosition()
{
	if (bLocal)
	{
		FVector velocity = GetRampVelocity();
		double curTimeStamp = GetClientTimestamp();
		
		FVector curPos = GetOwner()->GetActorLocation();
		EStateType state = curState;
		SendPosMsgIndex+=1;
		SendPosition_Server(curPos, velocity,
		                    curTimeStamp, state,SendPosMsgIndex);
		lastVelocity = velocity;
		clientLastSendPos = GetOwner()->GetActorLocation();
		lastSendTimeStamp = GetWorld()->TimeSeconds;
		if (movementComponent)
		{
			clientLastFloorNormal = movementComponent->CurrentFloor.HitResult.ImpactNormal;
		}
	}
}

void USyncMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	bServer ? ServerTick(DeltaTime) : ClientTick(DeltaTime);
}

void USyncMoveComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(USyncMoveComponent, ServerTimestamp, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(USyncMoveComponent, SendPosMsgIndex, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(USyncMoveComponent,curState,COND_SkipOwner);

}

void USyncMoveComponent::OnRep_ServerTimestamp()
{
	reciveServerTimestampWorldTime = GetWorld()->TimeSeconds;
	reciveServerTimestampPlatformTime = FPlatformTime::Seconds();
	offsetWorldTimestamp = reciveServerTimestampWorldTime - ServerTimestamp;
	offsetPlatformTimestamp = reciveServerTimestampPlatformTime - ServerTimestamp;
}


void USyncMoveComponent::ServerTick(float DeltaTime)
{
	if (bEnableServerAutoSend)
	{
		ServerAutoSend();
	}
	childActors.Empty();
	if (GetOwner())
	{
		GetOwner()->GetAllChildActors(childActors);
		for (auto childActor : childActors)
		{
			UActorComponent* com = childActor->GetComponentByClass(USyncMoveComponent::StaticClass());
			if (com)
			{
				USyncMoveComponent* sync = Cast<USyncMoveComponent>(com);
				if (sync && sync->GetFootPrintComponent() != nullptr)
				{
					sync->GetFootPrintComponent()->AddFootPrint(sync->GetRootPos(), FPlatformTime::Seconds(), (uint8)sync->curState);
				}
			}
		}
	}
	
	if (GetOwner()&&GetOwner()->GetAttachParentActor())
	{
		if (FootPrintComponent)
		{
			FootPrintComponent->AddFootPrint(GetRootPos(),FPlatformTime::Seconds(),(uint8)curState);
		}
	}
}

void USyncMoveComponent::ClientTick(float DeltaTime)
{
	if (!bEnableSync)
	{
		return;
	}
	UpdateNetRoleType();
	if (bLocal)
	{
		RegisterLocalTimer();
		if (CheatChecker&&CheatChecker->IsCollisionRadiusChange())
		{
			SendPosition();
		}
		else
		{
			if (bExtrapolation)
			{
				if (GetChangeFloorAngle() > 0.1 || GetChangeVelocityAngle() > 5 || GetChangeVelocitySize() > 50 )
				{
					/*UE_LOG(LogTemp, Error, TEXT("-------------change angle %f  change size %f "), GetChangeVelocityAngle(),
						   lastVelocity.Size() - GetVelocity().Size());*/
					SendPosition();
				}
			}
		}
		
	}
	else
	{
		UnRegisterLocalTimer();
	}
	if (bSimulated)
	{
		SetVelocity( (GetOwner()->GetActorLocation() - lastFramePos) / DeltaTime);
		ClientLerpPos(DeltaTime);
		if (lerpRotatorSpeed > 0)
		{
			ClientLerpRotation(DeltaTime);
		}
		else
		{
			if (curVelocity.SizeSquared() > 0.01 && bRotationFollowVelocity)
			{
				RotationFollowVelocity(DeltaTime);
			}
		}
	}
}
void USyncMoveComponent::ServerAutoSend()
{
	if (bEnableServerAutoSend)
	{
		ServerAutoPos();
		if (bSyncRotation&&!bRotationFollowVelocity)
		{
			ServerAutoRotation();
		}
	}
}

void USyncMoveComponent::ServerAutoPos()
{
	FVector curPos = root->GetComponentLocation();
	if (!SameVector(lastSendPos, curPos, 0.1))
	{
		
		lastSendPos = curPos;
		if (FootPrintComponent)
		{
			FootPrintComponent->AddFootPrint(curPos,FPlatformTime::Seconds(),0);
		}
		ServerSendPos_Multicast(curPos);
	}
}

void USyncMoveComponent::ServerAutoRotation()
{
	FVector curRotation = root->GetComponentRotation().Euler();
	if (!SameVector(lastSendRotaion, curRotation, 0.1))
	{
		lastSendRotaion = curRotation;
		ServerSendRotation_Multicast(int16(curRotation.X * 100), int16(curRotation.Y * 100),
									 int16(curRotation.Z * 100));
	}
}

float USyncMoveComponent::GetChangeVelocityAngle()
{
	FVector velocity = GetRampVelocity();
	if (lastVelocity.IsNearlyZero(0.1) && velocity.IsNearlyZero(0.1))
	{
		return 0;
	}
	float cos = UKismetMathLibrary::Dot_VectorVector(lastVelocity.GetSafeNormal(0.01),
	                                                 velocity.GetSafeNormal(0.01));
	return UKismetMathLibrary::DegAcos(cos);
}

float USyncMoveComponent::GetChangeFloorAngle()
{
	if (movementComponent)
	{
		return UKismetMathLibrary::Abs(
			movementComponent->CurrentFloor.HitResult.ImpactNormal.Z - clientLastFloorNormal.Z);
	}
	return 0;
}

float USyncMoveComponent::GetGroundSlopeAngle()
{
	float cos = UKismetMathLibrary::Dot_VectorVector(movementComponent->CurrentFloor.HitResult.ImpactNormal,
	                                                 FVector::UpVector);
	return UKismetMathLibrary::DegAcos(cos);
}

FVector USyncMoveComponent::GetGroundSlopeDir()
{
	FVector temp =  FVector::CrossProduct(movementComponent->CurrentFloor.HitResult.ImpactNormal, FVector::UpVector);
	temp.Normalize();
	FVector dir = FVector::CrossProduct(temp, movementComponent->CurrentFloor.HitResult.ImpactNormal);
	dir.Normalize();
	DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + 200 * dir, FColor::Blue,
	              false, 0.1, 0, 1);
	return dir;
}


float USyncMoveComponent::GetChangeVelocitySize()
{
	float lastSize = lastVelocity.Size();
	float curSize = GetVelocity().Size();
	return UKismetMathLibrary::Abs(lastSize - curSize);
}

bool USyncMoveComponent::IsLocalControl()
{
	if (GetOwner())
	{
		if (GetOwner()->HasLocalNetOwner())
		{
			return true;
		}
	}
	return false;
}


void USyncMoveComponent::ClientSendPosTimerFunc()
{
	if (!bEnableSync || !bLocal)
	{
		return;
	}
	FVector curPos = root->GetComponentLocation();

	if (!SameVector(curPos, clientLastSendPos, 0.1))
	{
		SendPosition();
	}
	
}

void USyncMoveComponent::ClientSendRotationTimerFunc()
{
	if (!bEnableSync || !bLocal)
	{
		return;
	}
	FRotator curRotator = root->GetComponentRotation();
	if (!clientLastSendRotator.Equals(curRotator, 1))
	{
		SendRotation_Server(curRotator);
		clientLastSendRotator = curRotator;
	}
}

FVector USyncMoveComponent::GetExtrapolationPos(FVector pos, FVector extrapolationVelocity)
{
	FVector extrapolationPos = pos + extrapolationVelocity * (PosSyncInterval + 0.1);
	FHitResult OutHit;
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, pos, extrapolationPos,
	                                                        UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel9),
	                                                        false, IgnoreActors
	                                                        , EDrawDebugTrace::None, OutHit
	                                                        , true);
	if (bHit)
	{
		extrapolationPos = OutHit.ImpactPoint;
	}
	return extrapolationPos;
}

void USyncMoveComponent::ServerFixPos(FVector pos,bool bSweep)
{
	if (root)
	{
		GetOwner()->SetActorLocation(pos, bSweep,nullptr,
							   root->IsSimulatingPhysics() ? ETeleportType::TeleportPhysics : ETeleportType::None);
		
		pos = GetOwner()->GetActorLocation();
	}
	RefreshSendPosMsgIndex_Multicast();
	ServerSetPos_Multicast(pos);
	
}

void USyncMoveComponent::ServerSendMsgIndex_Multicast_Implementation(uint32 msgIndex)
{
	SendPosMsgIndex = msgIndex;
}

void USyncMoveComponent::RefreshSendPosMsgIndex_Multicast_Implementation()
{
	SendPosMsgIndex+=10;
	FCoreDelegates::OnObjectNetPropertyChanged.Broadcast(this);
}


FVector USyncMoveComponent::GetRampVelocity()
{
	if (curState == EStateType::InAir)
	{
		return GetVelocity();
	}
	if (movementComponent)
	{
		const FVector velocity = GetVelocity();
		const FVector FloorNormal = movementComponent->CurrentFloor.HitResult.ImpactNormal;
		if (FloorNormal.Z < (1.f - KINDA_SMALL_NUMBER) && FloorNormal.Z > KINDA_SMALL_NUMBER)
		{
			const float FloorDotDelta = (FloorNormal | velocity);
			const FVector RampVelocity(velocity.X, velocity.Y, -FloorDotDelta / FloorNormal.Z);
			return RampVelocity.GetSafeNormal() * velocity.Size();
		}
		return GetVelocity();
	}
	return GetVelocity();
}


void USyncMoveComponent::ServerSetPos_Multicast_Implementation(FVector pos)
{
	if (root)
	{
		root->SetWorldLocation(pos, false, nullptr,
							   root->IsSimulatingPhysics() ? ETeleportType::TeleportPhysics : ETeleportType::None);
		if (bLocal)
		{
			CheatChecker->SelfPullBacked();
			UE_LOG(LogTemp,Warning,TEXT("Pullback %s Name:%s"),*pos.ToString(),*GetOwner()->GetName());
		}
		else if (bServer)
		{
			FCoreDelegates::OnObjectNetPropertyChanged.Broadcast(this);
		}
	}
	ServerSendPos_Multicast_Implementation(pos);
}

FVector USyncMoveComponent::GetVelocity()
{
	if (IsLocalControl())
	{
		return GetOwner()->GetVelocity();
	}
	return curVelocity;
}


void USyncMoveComponent::SendRotation_Server_Implementation(FRotator rotator)
{
	if (!UKismetMathLibrary::InRange_FloatFloat(rotator.Pitch,PitchLimit.X,PitchLimit.Y))
	{
		return;
	}
	if (!UKismetMathLibrary::InRange_FloatFloat(rotator.Roll,RollLimit.X,RollLimit.Y))
	{
		return;
	}
	root->SetWorldRotation(rotator,false,nullptr,root->IsSimulatingPhysics() ? ETeleportType::ResetPhysics : ETeleportType::None);
	SendRotation();
}

void USyncMoveComponent::SendPosition_Server_Implementation(
	FVector_NetQuantize100 pos, FVector_NetQuantize100 velocity, double timestamp, EStateType state,uint32 sendPosMsgIndex)
{
	if (CheatChecker == nullptr)
	{
		return;
	}
	FVector curPos = GetRootPos();
	if (bEnableCheck)
	{
		if (sendPosMsgIndex<SendPosMsgIndex)
		{
			//UE_LOG(LogTemp, Warning, TEXT("sendPosMsgIndex<SendPosMsgIndex clientMsgIndex:%d, serverMsgIndex:%d"),sendPosMsgIndex,SendPosMsgIndex);
			return;
		}
		if (!IsTimestampOk(timestamp))
		{
			ServerFixPos(curPos);
			return;
		}
	
		if (IsOverSpeed(curPos, pos, timestamp)  ||
			!CheatChecker->PassSwimStateCheck(state) || CheatChecker->IsTargetPosFly(pos, state) ||
			!CheatChecker->IsClimbPosOk(pos, state) || !CheatChecker->IsLadderPosOk(pos, state)|| !CheatChecker->IsCurvePosOk(pos,state,timestamp))
		{
			ServerFixPos(curPos);
			return;
		}
		if (IsThrowWall(curPos, pos))
		{
			const UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(this->root);
			if (CapsuleComponent)
			{
				ServerFixPos(curPos-GetOwner()->GetActorForwardVector()*CapsuleComponent->GetScaledCapsuleRadius()*1.2);
			}
			else
			{
				ServerFixPos(curPos-GetOwner()->GetActorForwardVector());
			}
			return;
		}
		if (lastState != EStateType::InAir && state == EStateType::InAir)
		{
			if (velocity.Z > CheatChecker->MaxUpSpeed)
			{
				velocity.Z = CheatChecker->MaxUpSpeed;
			}
			CheatChecker->enterAirVelocity = velocity;
			CheatChecker->enterAirPos = pos;
			CheatChecker->enterAirTimeStamp = timestamp;
		}
		if (!CheatChecker->IsAirPosOk(pos, timestamp, state))
		{
			bool hasGround = false;
			ServerFixPos(CheatChecker->GetGroundPos(pos, hasGround));
			lastState =  EStateType::Ground;
			return;
		}
		if (velocity.Size2D() > CheatChecker->MaxSpeed)
		{
			velocity.X = velocity.GetSafeNormal().X * CheatChecker->MaxSpeed;
			velocity.Y = velocity.GetSafeNormal().Y * CheatChecker->MaxSpeed;
		}
		if (velocity.Z > CheatChecker->MaxUpSpeed)
		{
			velocity.Z = CheatChecker->MaxUpSpeed;
		}
	}
	SendPosMsgIndex = sendPosMsgIndex;
	lastReciveTimestamp = timestamp;
	SetCurState(state);
	if (lastState != EStateType::InAir && curState == EStateType::InAir)
	{
		CheatChecker->enterAirVelocity = velocity;
		CheatChecker->enterAirPos = pos;
		CheatChecker->enterAirTimeStamp = timestamp;
	}
	if (lastState != EStateType::BackTo && curState == EStateType::BackTo)
	{
		CheatChecker->startCurveMovePos = pos;
		CheatChecker->startCurveMoveTimeStamp = timestamp;
	}
	lastVelocity = curVelocity;
	SetVelocity(velocity);
	GetOwner()->SetActorLocation(pos, false, nullptr,
						   root->IsSimulatingPhysics() ? ETeleportType::TeleportPhysics : ETeleportType::None);
	AddHistoryPosInfo(timestamp,pos);
	if (FootPrintComponent)
	{
		FootPrintComponent->AddFootPrint(pos,FPlatformTime::Seconds(),(uint8)state);
	}
	FCoreDelegates::OnObjectNetPropertyChanged.Broadcast(this);
	if (!SameVector(lastSendPos, GetRootPos(), 0.1))
	{
		if (bExtrapolation)
		{
			ServerSendPos_Multicast(GetExtrapolationPos(pos, velocity));
		}
		else
		{
			ServerSendPos_Multicast(pos);
		}
	}
	else
	{
		SetVelocity( FVector::ZeroVector);
	}
}


void USyncMoveComponent::ClientLerpPos(float DeltaTime)
{
	const FVector curPos = root->GetComponentLocation();
	if (SameVector(curPos, lastReceivedPos, 0.1))
	{
		SetVelocity( FVector::ZeroVector);
	}
	else
	{
		root->SetWorldLocation(FMath::VInterpConstantTo(root->GetComponentLocation(), lastReceivedPos, DeltaTime,
		                                                lerpPosSpeed), false, nullptr,
		                       root->IsSimulatingPhysics() ? ETeleportType::TeleportPhysics : ETeleportType::None);
	}
	lastFramePos = curPos;
}

void USyncMoveComponent::ClientLerpRotation(float DeltaTime)
{
	const FQuat curRotator = root->GetComponentTransform().GetRotation();
	if (SameVector(curRotator.Vector(), lastReceivedRotation.Vector(), 0.01))
	{
		lerpRotatorSpeed = 0;
	}
	else
	{
		const FRotator r = FMath::QInterpTo(curRotator, lastReceivedRotation.Quaternion(),
														DeltaTime, lerpRotatorSpeed).Rotator();
		root->SetWorldRotation(r,false,nullptr,root->IsSimulatingPhysics() ? ETeleportType::ResetPhysics : ETeleportType::None);
	}
}

bool USyncMoveComponent::SameVector(FVector one, FVector two, float threshold)
{
	if (FMath::Abs(one.X - two.X) > threshold) return false;
	if (FMath::Abs(one.Y - two.Y) > threshold) return false;
	if (FMath::Abs(one.Z - two.Z) > threshold) return false;
	return true;
}

void USyncMoveComponent::RotationFollowVelocity(float DeltaTime)
{
	const FRotator curRotator = root->GetComponentRotation();
	FRotator velocityRotator = curVelocity.Rotation();
	LimitRotation360(velocityRotator);

	if (bRotationFollowVelocityIgnoreRoll)
	{
		velocityRotator.Roll = 0;
	}
	if (bRotationFollowVelocityIgnorePitch)
	{
		velocityRotator.Pitch = 0;
	}
	if (bRotationFollowVelocityIgnoreYaw)
	{
		velocityRotator.Yaw = 0;
	}

	root->SetWorldRotation(FMath::QInterpConstantTo(curRotator.Quaternion(), velocityRotator.Quaternion(),
	                                                DeltaTime, 5).Rotator(),false,nullptr,root->IsSimulatingPhysics() ? ETeleportType::ResetPhysics : ETeleportType::None);
}

void USyncMoveComponent::UpdateNetRoleType()
{
	if (!bServer)
	{
		if (IsLocalControl())
		{
			bLocal = true;
			bSimulated = false;
		}
		else
		{
			bLocal = false;
			bSimulated = true;
		}
	}
}

void USyncMoveComponent::RegisterLocalTimer()
{
	if (!clientSendPosTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(clientSendPosTimerHandle, this,
		                                       &USyncMoveComponent::ClientSendPosTimerFunc, PosSyncInterval, true);
	}
	if (!clientSendRotationTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(clientSendRotationTimerHandle, this,
		                                       &USyncMoveComponent::ClientSendRotationTimerFunc,
		                                       RotationSyncInterval, true);
	}
}

void USyncMoveComponent::UnRegisterLocalTimer()
{
	if (clientSendPosTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(clientSendPosTimerHandle);
	}
	if (clientSendRotationTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(clientSendRotationTimerHandle);
	}
}

bool USyncMoveComponent::IsThrowWall(FVector curPos, FVector targetPos)
{
	FHitResult OutHit;
	bool bHit = true;
	const UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(this->root);
	if (CapsuleComponent)
	{
		float radius = CapsuleComponent->GetScaledCapsuleRadius() * 0.5;
		float halfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight() * 0.5;
		if (curState == EStateType::Ladder)
		{
			radius *= 0.1f;
			radius *= 0.1f;
		}
		bHit = UKismetSystemLibrary::CapsuleTraceSingle(this, curPos, targetPos,
		                                                radius,
		                                                halfHeight,
		                                                UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel9), false,
		                                                IgnoreActors
		                                                , EDrawDebugTrace::ForDuration, OutHit
		                                                , true
		                                                , FLinearColor::White, FLinearColor::Red
		                                                , 1.f);
	}
	else
	{
		const UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(this->root);
		if (Primitive)
		{
			bHit = UKismetSystemLibrary::BoxTraceSingle(this, curPos, targetPos,
			                                            Primitive->Bounds.BoxExtent * 0.5 * 0.7,
			                                            Primitive->GetComponentRotation(),
			                                            UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel9), false,
			                                            IgnoreActors
			                                            , EDrawDebugTrace::None, OutHit
			                                            , true
			                                            , FLinearColor::White, FLinearColor::Red
			                                            , 5.f);
		}
	}
	if (OutHit.GetActor())
	{
		if (bEnableLog)
		{
			UE_LOG(LogTemp, Warning, TEXT("Throw Wall Actor Name%s curPos:%s targetPos:%s "), *OutHit.GetActor()->GetName(),
			   *curPos.ToString(), *targetPos.ToString());
			if (OutHit.GetComponent())
			{
				UE_LOG(LogTemp, Warning, TEXT("Throw Wall Component Name%s "), *OutHit.GetComponent()->GetName());
			}
		}
	}

	return bHit;
}

bool USyncMoveComponent::IsOverSpeed(FVector curPos, FVector targetPos, double clientTimestamp)
{
	if (CheatChecker == nullptr)
	{
		return true;
	}
	float moveDis = FVector::Dist2D(curPos, targetPos);
	
	double time = clientTimestamp - lastReciveTimestamp;
	if (time == 0)
	{
		if (IsOverSpeedByHistoryPosInfo(targetPos))
		{
			return true;
		}
		return false;
	}
	if (time>(PosSyncInterval+0.1))
	{
		time = PosSyncInterval+0.1;
	}
	double speed = moveDis / time;
	if (speed > CheatChecker->MaxSpeed||moveDis>CheatChecker->MaxSpeed)
	{
		UE_LOG(LogTemp, Warning, TEXT("OverSpeed Want Speed   %f  UseTime: %f, MoveDis: %f"), speed,
		       time,moveDis)
		return true;
	}
	return false;
}

bool USyncMoveComponent::IsOverSpeedByHistoryPosInfo(FVector targetPos)
{
	 if (HistoryPosInfoDeque.Num()>0)
	{
		 const FHistoryPosInfo& OldInfo = HistoryPosInfoDeque.First();
		float moveDis = FVector::Dist2D(OldInfo.Pos, targetPos);
		if (moveDis>CheatChecker->MaxSpeed)
		{
			UE_LOG(LogTemp, Warning, TEXT("IsOverSpeedByHistoryPosInfo %f"),moveDis);
			return true;
		}
	}
	return false;
}

bool USyncMoveComponent::IsTimestampOk(double timestamp)
{
	double curTimestamp = FPlatformTime::Seconds();
	if ( curTimestamp - timestamp < -0.1f||curTimestamp - timestamp > 2 || timestamp < lastReciveTimestamp)
	{
		if (bEnableLog)
		{
			UE_LOG(LogTemp, Warning,
		   TEXT(
			   "Timestamp Error --------- curServerTimestamp:%f , clientTimestamp:%f,  dif %f:, lastReciveTimestamp %f"
		   ),
		   curTimestamp, timestamp, curTimestamp - timestamp, lastReciveTimestamp);
		}
		ServerTimestamp = curTimestamp;
		return false;
	}
	return true;
}

void USyncMoveComponent::AddHistoryPosInfo(double timestamp, FVector pos)
{
	const FHistoryPosInfo HistoryPosInfo(pos,timestamp);
	// HistoryPosInfoDeque.EmplaceLast(HistoryPosInfo);
	while(true)
	{
		// const FHistoryPosInfo& OldInfo = HistoryPosInfoDeque.First();
		// if(timestamp - OldInfo.Timestamp > 1)
		// {
		// 	HistoryPosInfoDeque.PopFirst();
		// }
		// else
		{
			break;
		}
	}
}

float USyncMoveComponent::GetClientTimestamp()
{
	return GetWorld()->TimeSeconds - offsetWorldTimestamp;
}

float USyncMoveComponent::GetClientPlatformTimestamp()
{
	return FPlatformTime::Seconds() - offsetPlatformTimestamp;
}


void USyncMoveComponent::LimitRotation360(FRotator& rotator)
{
	if (rotator.Roll < 0)
	{
		rotator.Roll += 360;
	}
	if (rotator.Yaw < 0)
	{
		rotator.Yaw += 360;
	}
	if (rotator.Pitch < 0)
	{
		rotator.Pitch += 360;
	}
}

void USyncMoveComponent::CalculateLerpVelocity()
{
	if (root)
	{
		const FVector Delta = lastReceivedPos - root->GetComponentLocation();
		//curVelocity = Delta / (PosSyncInterval + NetworkShakeTolerance);
		lerpPosSpeed = Delta.Size() / (PosSyncInterval + NetworkShakeTolerance);
	}
}

void USyncMoveComponent::ServerSendPos_Multicast_Implementation(FVector pos)
{
	if (bServer)
	{
		lastSendPos = pos;
	}
	lastReceivedPos = pos;
	if (bSimulated)
	{
		bReachRecivedPos = false;
		CalculateLerpVelocity();
	}
}

void USyncMoveComponent::SendRotation()
{
	FVector curRotation = root->GetComponentRotation().Euler();
	if (!SameVector(lastSendRotaion, curRotation, 0.5))
	{
		lastSendRotaion = curRotation;
		ServerSendRotation_Multicast(int16(curRotation.X * 100), int16(curRotation.Y * 100),
		                             int16(curRotation.Z * 100));
	}
}

void USyncMoveComponent::SetCurState(EStateType state)
{
	if (CheatChecker != nullptr)
	{
		lastState = curState;
		curState = state;
		CheatChecker->SetCurState(state);
		if (FootPrintComponent)
		{
			FootPrintComponent->AddFootPrint(GetRootPos(),FPlatformTime::Seconds(),(uint8)state);
		}
	}
}

EStateType USyncMoveComponent::GetCurState()
{
	return curState;
}

void USyncMoveComponent::SetEnableServerAutoSend(bool enable)
{
	bEnableServerAutoSend = enable;
}

void USyncMoveComponent::RefreshMsgIndex()
{
	ServerSendMsgIndex_Multicast(SendPosMsgIndex);
}

void USyncMoveComponent::ServerSendRotation_Multicast_Implementation(int16 roll, int16 pitch, int16 yaw)
{
	FRotator r;
	r.Roll = roll / 100;
	r.Pitch = pitch / 100;
	r.Yaw = yaw / 100;
	lastReceivedRotation = r;
	if (bSimulated)
	{
		if (root)
		{
			const FQuat curRotator = root->GetComponentTransform().GetRotation();
			lerpRotatorSpeed = curRotator .AngularDistance(lastReceivedRotation.Quaternion())/ (RotationSyncInterval*1.2f);
		}
	}
}
