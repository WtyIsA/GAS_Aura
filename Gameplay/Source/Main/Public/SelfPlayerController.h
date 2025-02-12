#pragma once

#include "GameFramework/PlayerController.h"
#include "GameplayEffect.h"
#include "SelfPlayerController.generated.h"

class USessionMgr;

UCLASS()
class ASelfPlayerController : public APlayerController
{
    GENERATED_BODY()
    
public:
    ASelfPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void PawnLeavingGame() override;

    UFUNCTION(Server, Reliable)
    void SendToServer(const TArray<uint8>& data);
    UFUNCTION(Client, Reliable)
    void SendToClient(const TArray<uint8>& data);
    UFUNCTION(NetMulticast, Reliable)
    void SendToAllClients(const TArray<uint8>& data);
    UFUNCTION()
    FString GetConnectIp(int32& nPort);
    UFUNCTION()
    void SetSessionMgr(USessionMgr* pMgr);

    virtual void OnNetCleanup(class UNetConnection* Connection) override;

    UFUNCTION(BlueprintImplementableEvent)
    void LuaOnNetCleanup();
    
    UFUNCTION(Client, Reliable)
    void ShowDamageNumber(float DamageAmount, AActor* TargetCharacter, FVector Pos, const FGameplayEffectSpec& GESpec);
    void ShowDamageNumber_Implementation(float DamageAmount, AActor* TargetCharacter, FVector Pos, const FGameplayEffectSpec& GESpec);
    UFUNCTION(BlueprintImplementableEvent)
    void K2_ShowDamageNumber(float DamageAmount, AActor* TargetCharacter, FVector Pos, const FGameplayEffectSpec& GESpec);
    void Send(int MsgId,void* Msg);
    void Multicast(int MsgId,void* Msg);
    
    virtual void SetPawn(APawn* InPawn) override;
    virtual void Tick( float DeltaSeconds ) override;
    UPROPERTY()
    int64 RoleId;

    virtual void NotifyActorChannelFailure(UActorChannel* ActorChannel) override;
    virtual bool IsLocalController() const override;

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPawnChanged, APlayerController*);
    static FOnPawnChanged OnPawnChanged;

    virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<class AActor*>& ActorList) override;

    virtual void SeamlessTravelTo(class APlayerController* NewPC) override;

    UFUNCTION()
    void LuaFlushPressedKeys();
private:
    void DealArrayMsg(const TArray<uint8>& data);
    USessionMgr*    m_pSessionMgr;
protected:
    /** Pawn has been possessed, so changing state to NAME_Playing. Start it walking and begin playing with it. */
    virtual void BeginPlayingState() override;
    /** Leave playing state. */ 
    virtual void EndPlayingState() override;
    void OnGEAppliedToTarget(class UAbilitySystemComponent* Target, const struct FGameplayEffectSpec& GESpec, struct FActiveGameplayEffectHandle ActiveGEHandle);

    class ACustomPlayerState* PS;
    class FDelegateHandle OnGEAppliedDelegateHandle;
};
