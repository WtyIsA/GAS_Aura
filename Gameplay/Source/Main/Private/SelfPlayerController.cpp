#include "SelfPlayerController.h"

#include "AbilitySystemComponent.h"
#include "LuaManger.h"
#include "UnLuaEx.h"
#include "SelfGameMode.h"
#include "msg/SLuaMsgInfo.h"
#include "Net/MessagePool.h"
#include "Net/SessionMgr.h"
#include "SignificanceManager.h"
#include "AbilitySystem/CustomGameplayEffectTypes.h"
#include "AbilitySystem/CustomPlayerState.h"
#include "AbilitySystem/Attributes/CombatAttributeSet.h"

ASelfPlayerController::FOnPawnChanged ASelfPlayerController::OnPawnChanged;

ASelfPlayerController::ASelfPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShowMouseCursor = true;
}

void ASelfPlayerController::PawnLeavingGame()
{
    
}

void ASelfPlayerController::SetSessionMgr(USessionMgr* pMgr)
{
	m_pSessionMgr = pMgr;
}

void ASelfPlayerController::OnNetCleanup(UNetConnection* Connection)
{
	Super::OnNetCleanup(Connection);
	LuaOnNetCleanup();
}

void ASelfPlayerController::SendToServer_Implementation(const TArray<uint8>& data)
{
	DealArrayMsg(data);
}

void ASelfPlayerController::SendToClient_Implementation(const TArray<uint8>& data)
{
	DealArrayMsg(data);
}

void ASelfPlayerController::SendToAllClients_Implementation(const TArray<uint8>& data)
{
	DealArrayMsg(data);
}

void ASelfPlayerController::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);
	if(IsRunningDedicatedServer())
	{
		if(APawn* PawnPtr = GetPawn())
		{
			ActorList.Add(PawnPtr);
		}
	}
}

void ASelfPlayerController::SeamlessTravelTo(APlayerController* NewPC)
{
	Super::SeamlessTravelTo(NewPC);
	if(ASelfPlayerController* SelfPlayerController = Cast<ASelfPlayerController>(NewPC))
	{
		SelfPlayerController->RoleId = RoleId;
	}
}

void ASelfPlayerController::LuaFlushPressedKeys()
{
	FlushPressedKeys();
}

void ASelfPlayerController::DealArrayMsg(const TArray<uint8>& data)
{
	FUnMarshal*  msg = CMessagePool::Get()->GetUnMarshal();
	msg->AttachArray(data,true);
	if(m_pSessionMgr)
		m_pSessionMgr->RecvMsg(RoleId, msg);
	CMessagePool::Get()->RecycleMessage(msg);
}

void ASelfPlayerController::BeginPlayingState()
{
	PS = GetPlayerState<ACustomPlayerState>();
	if (PS)
	{
		OnGEAppliedDelegateHandle = PS->GetAbilitySystemComponent()->OnGameplayEffectAppliedDelegateToTarget.AddUObject(this, &ASelfPlayerController::OnGEAppliedToTarget);
	}
}

void ASelfPlayerController::EndPlayingState()
{
	if (PS)
	{
		PS->GetAbilitySystemComponent()->OnGameplayEffectAppliedDelegateToTarget.Remove(OnGEAppliedDelegateHandle);
		OnGEAppliedDelegateHandle.Reset();
	}
}

void ASelfPlayerController::OnGEAppliedToTarget(UAbilitySystemComponent* Target, const FGameplayEffectSpec& GESpec, FActiveGameplayEffectHandle ActiveGEHandle)
{
	if (PS)
	{
		for(auto It : GESpec.ModifiedAttributes)
		{
			if (It.Attribute == PS->GetCombatAttributeSet()->GetDamageAttribute() && It.TotalMagnitude > 0.f)
			{
				TOptional<FVector> HitPos;
				const FHitResult* HitResult = GESpec.GetContext().GetHitResult();
				if (HitResult==nullptr)
				{
					FCustomGameplayEffectContext* EffectContext = static_cast<FCustomGameplayEffectContext*>(GESpec.GetContext().Get());
					if (EffectContext)
					{
						FGameplayAbilityTargetData* TargetData = EffectContext->GetTargetData().Get(0);
						if (TargetData)
						{
							HitResult = TargetData->GetHitResult();
							if (HitResult->ImpactPoint.IsNearlyZero() && TargetData->GetActors().Num() > 0)
							{
								HitPos = TargetData->GetActors()[0]->GetActorLocation();
							}
						}
					}
				}
				ShowDamageNumber(It.TotalMagnitude, Target->GetAvatarActor(), HitPos.IsSet() ? HitPos.GetValue() : (HitResult ? (FVector)HitResult->ImpactPoint :Target->GetAvatarActor()->GetActorLocation()), GESpec);
				break;
			}
		}
	}
}

void ASelfPlayerController::ShowDamageNumber_Implementation(float DamageAmount, AActor* TargetCharacter, FVector Pos, const FGameplayEffectSpec& GESpec)
{
	K2_ShowDamageNumber(DamageAmount, TargetCharacter, Pos, GESpec);
}

void ASelfPlayerController::Send(int MsgId, void* Msg)
{
	if(!m_pSessionMgr)
	{
		UE_LOG(GAME,Warning,TEXT("[msg] ASelfPlayerController send, sessionmgr is null, why?"));
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("session is null");
		return;
	}
	Table* TablePtr = (Table*)Msg;    
	if(TablePtr != nullptr)
	{
		FMarshal* Obs = CMessagePool::Get()->GetMarshal();
		Obs->Write(MsgId);
		if(!m_pSessionMgr->Marshal(TablePtr, Obs, MsgId))
		{
			UE_LOG(GAME, Error,TEXT("[net]Marshal,error,msgid=%d"),MsgId);
			return;
		}
		Obs->Pack();
		Obs->RemoveNotUser();
		if(IsRunningDedicatedServer())
		{
			SendToClient(Obs->GetBytes());
		}
		else
		{
			SendToServer(Obs->GetBytes());
		}		
		///TODO:Send
		if(USessionMgr::m_bShowMessageLog)
		{
			SLuaMsgInfo* pInfo = m_pSessionMgr->GetMsgInfo(MsgId);
			UE_LOG(GAME, Display, TEXT("[msg]send msg id=%d, len=%d, name=%s"), MsgId, Obs->Length(), ANSI_TO_TCHAR(pInfo->m_msgName.c_str()));
		}
		CMessagePool::Get()->RecycleMessage(Obs);
	}
}

void ASelfPlayerController::Multicast(int MsgId,void* Msg)
{
#if !UE_SERVER
	UE_LOG(GAME,Warning,TEXT("[net]Multicast in Client"))
	return;
#endif

	if(!m_pSessionMgr)
	{
		UE_LOG(GAME,Warning,TEXT("[msg] ASelfPlayerController Multicast, sessionmgr is null, why?"));
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("session is null");
		return;
	}
	
	Table* TablePtr = (Table*)Msg;    
	if(TablePtr != nullptr)
	{
		FMarshal* Obs = CMessagePool::Get()->GetMarshal();
		Obs->Write(MsgId);
		if(!m_pSessionMgr->Marshal(TablePtr, Obs, MsgId))
		{
			UE_LOG(GAME, Error,TEXT("[net]Marshal,error,msgid=%d"),MsgId);
			return;
		}
		Obs->Pack();
		Obs->RemoveNotUser();
		SendToAllClients(Obs->GetBytes());

		if(USessionMgr::m_bShowMessageLog)
		{
			SLuaMsgInfo* pInfo = m_pSessionMgr->GetMsgInfo(MsgId);
			UE_LOG(GAME, Display, TEXT("[msg]send msg id=%d, len=%d, name=%s"), MsgId, Obs->Length(), ANSI_TO_TCHAR(pInfo->m_msgName.c_str()));
		}

		CMessagePool::Get()->RecycleMessage(Obs);
	}
}

void ASelfPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if(IsRunningDedicatedServer())
	{
		ASelfPlayerController::OnPawnChanged.Broadcast(this);
	}
}

void ASelfPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (IsLocalController())
	{
		UWorld* MyWorld = GetWorld();
		if (MyWorld!=nullptr)
		{
			USignificanceManager*  SignificanceManager = FSignificanceManagerModule::Get(GetWorld());
			if (SignificanceManager)
			{
				FVector ViewLocation;
				FRotator ViewRotation;
				GetPlayerViewPoint(ViewLocation,ViewRotation);
				TArray<FTransform> Viewpoints;
				Viewpoints.Emplace(ViewRotation,ViewLocation,FVector::OneVector);
				SignificanceManager->Update(Viewpoints);
			}
		}
	}
}

void ASelfPlayerController::NotifyActorChannelFailure(UActorChannel* ActorChannel)
{
	if(ActorChannel && ActorChannel->Actor && IsValid(ActorChannel->Actor))
	{
		ULuaManger::Get()->NotifyReplicateActorFail(this,ActorChannel->Actor);
	}
}

bool ASelfPlayerController::IsLocalController() const
{
	if (Player && NetConnection)
	{
		return Super::IsLocalController();
	}
	const ENetMode NetMode = GetNetMode();
	if (NetMode == NM_Standalone)
	{
		// Not networked.
		return true;
	}
	if (NetMode == NM_Client && GetLocalRole() == ROLE_AutonomousProxy)
	{
		// Networked client in control.
		return true;
	}
	if (GetRemoteRole() > ROLE_SimulatedProxy && GetLocalRole() == ROLE_Authority)
	{
		// For Offline Player
		return true;
	}
	return false;
}

FString ASelfPlayerController::GetConnectIp(int32& nPort)
{
	if(IsValid(NetConnection) && NetConnection->RemoteAddr)
	{		
		NetConnection->RemoteAddr->GetPort(nPort);
		return NetConnection->RemoteAddr->ToString(false);		
	}
	nPort = 0;
	return TEXT("");
}


BEGIN_EXPORT_REFLECTED_CLASS(ASelfPlayerController)
	ADD_FUNCTION(Send)
    ADD_FUNCTION(Multicast)
END_EXPORT_CLASS()
IMPLEMENT_EXPORTED_CLASS(ASelfPlayerController)