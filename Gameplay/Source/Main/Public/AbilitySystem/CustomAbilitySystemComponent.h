#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CustomAnimNotifyState.h"
#include "CustomAnimNotify.h"
#include "UnLuaInterface.h"
#include "CustomAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FAnimNotifyStateDelegate, UCustomAnimNotifyState*, const FName&);
DECLARE_MULTICAST_DELEGATE_OneParam(FAnimNotifyDelegate, UCustomAnimNotify*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicAbilityDelegate, const UGameplayAbility*, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAbilityEndDelegate, bool, bWasCancelled, FGameplayAbilitySpecHandle, Handle, UGameplayAbility*, Ability);

/** Delegate for when an effect is applied */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGameplayEffectAppliedDynamicDelegate, UAbilitySystemComponent*, Source, const FGameplayEffectSpec&, SpecApplied, FActiveGameplayEffectHandle, ActiveHandle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPreGameplayEffectApplyToSelfDelegate, const FGameplayEffectSpec&, SpecToApply);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPreGameplayEffectApplyToTargetDelegate, UAbilitySystemComponent*, Target, const FGameplayEffectSpec&, SpecToApply);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnImmunityBlockGE, const FGameplayEffectSpec&, BlockedSpec, const FActiveGameplayEffect, ImmunityGameplayEffect);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnShieldChangeDelegate, FActiveGameplayEffectHandle,
                                              GEHandle, const FGameplayEffectSpec, Spec, float, PrevValue, float, NewValue);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnTagCountChanged, const FGameplayTag&, Tag, int32, Count);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplayEffectRemovedDynamicDelegate, const FActiveGameplayEffect&, AGE);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnGameplayAttributeValueChangeDelegate, float, OldValue, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FGEApplyDelegate, UAbilitySystemComponent*, TargetASC, const FGameplayEffectSpec, EffectSpec, FGameplayModifierEvaluatedData, EvaluatedData, float&, ToApplyDamage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityEndedDelegate, const FAbilityEndedData&, Ability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGameplayTagChangeDelegate, const FGameplayTag&, Tag, bool, TagExists);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCustomAbilityFailedDelegate, const FGameplayAbilitySpecHandle, Handle, const FGameplayTagContainer&, Container);

struct ShieldInfo
{
	FActiveGameplayEffectHandle GEHandle;
	float RemainValue;
	bool operator==(const ShieldInfo& Other) const
	{
		return GEHandle == Other.GEHandle;
	}

	bool operator!=(const ShieldInfo& Other) const
	{
		return GEHandle != Other.GEHandle;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAIN_API UCustomAbilitySystemComponent : public UAbilitySystemComponent,public IUnLuaInterface
{
	GENERATED_BODY()

public:
	UCustomAbilitySystemComponent();
	virtual FString GetModuleName_Implementation() const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void NotifyAbilityCommit(UGameplayAbility* Ability) override;
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	virtual void NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;

	float CustomPlayMontage(UGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None, float StartTimeSeconds = 0.0f, bool bStopAllMontages =true);
	
	UFUNCTION(BlueprintCallable)
	void RemoveAllActiveGE();
	
	UFUNCTION(BlueprintCallable, Category = "GameplayCue",
		Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);
	
	UFUNCTION(BlueprintCallable, Category = "GameplayCue",	Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);
	
	UFUNCTION(BlueprintCallable, Category = "GameplayCue", Meta = (AutoCreateRefTerm = "GameplayCueParameters", GameplayTagFilter = "GameplayCue"))
	void RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION()
	void K2_ExecuteGameplayCue(const FGameplayTag GameplayCueTag, FGameplayEffectContextHandle EffectContext = FGameplayEffectContextHandle());
	UFUNCTION()
	void K2_ExecuteGameplayCue_WithParams(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);

	UFUNCTION()
	void K2_AddGameplayCue(const FGameplayTag GameplayCueTag, FGameplayEffectContextHandle EffectContext = FGameplayEffectContextHandle());
	UFUNCTION()
	void K2_AddGameplayCue_WithParams(const FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters);
	UFUNCTION()
	void K2_RemoveGameplayCue(const FGameplayTag GameplayCueTag, bool isPrediction = false, int32 id = -1);

	UFUNCTION(BlueprintCallable, Category = "GameplayTag")
	void RegisterGameplayTagChangeEvent(const FGameplayTag& Tag, const FOnTagCountChanged& Callback, EGameplayTagEventType::Type EventType = EGameplayTagEventType::NewOrRemoved);

	UFUNCTION(BlueprintCallable, Category = "GameplayTag")
	void UnRegisterGameplayTagChangeEvent(const FGameplayTag& Tag, EGameplayTagEventType::Type EventType = EGameplayTagEventType::NewOrRemoved);
	
	UFUNCTION(BlueprintCallable, Category = "GameplayTag")
	void K2_AddLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count=1);

	UFUNCTION(BlueprintCallable, Category = "GameplayTag")
	void K2_RemoveLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count = 1);

	/** Cancel all abilities with the specified tags. Will not cancel the Ignore instance */
	UFUNCTION(BlueprintCallable, Category = "GameplayAbility")
	void CancelAbilityByClass(UGameplayAbility* Ability);

	UFUNCTION(BlueprintCallable, Category = "GameplayAbility")
	void	K2_RemoteEndOrCancelAbility(FGameplayAbilitySpecHandle AbilityToEnd, FGameplayAbilityActivationInfo ActivationInfo, bool bWasCanceled);
	
	UFUNCTION(BlueprintCallable, Category = "GameplayAbility")
	void K2_CancelAllAbilities(UGameplayAbility* Ignore=nullptr);
	
	UFUNCTION()
	FGameplayAbilitySpecHandle K2_GiveAbility(TSubclassOf<UGameplayAbility> Ability,int32 AbilityID =0 ,int32 Lv = 0, int32 InputID = -1);
	UFUNCTION()
	FGameplayAbilitySpec K2_FindAbilitySpecFromHandle(FGameplayAbilitySpecHandle Handle);

	UFUNCTION()
	bool K2_TryActivateAbilityBySpecHandle(FGameplayAbilitySpecHandle AbilityToActivate, bool bAllowRemoteActivation = true);
	UFUNCTION()
	void K2_RemoveAbility(const FGameplayAbilitySpecHandle& Handle);
	UFUNCTION()
	const UGameplayEffect* K2_GetGameplayEffectDefForHandle(FActiveGameplayEffectHandle Handle);
	UFUNCTION()
	FGameplayEffectSpec K2_GetActiveGameplayEffectSpec(const FActiveGameplayEffectHandle Handle) const;
	
	void PreGEExecute(UAbilitySystemComponent& TargetASC, const FGameplayEffectSpec& EffectSpec, FGameplayModifierEvaluatedData& EvaluatedData, float& ToApplyDamage);
	void PostGEExecute(UAbilitySystemComponent& TargetASC, const FGameplayEffectSpec& EffectSpec, FGameplayModifierEvaluatedData& EvaluatedData, float AppliedDamage);

	UFUNCTION()
	bool K2_IsCurShield(FActiveGameplayEffectHandle GEHandle);
	TArray<ShieldInfo*>& GetShieldInfos();
	float GetRemainDamageWithShieldValue(float Damage, const FGameplayEffectSpec& Spec);
	
	UFUNCTION()
	void SetMoveSpeedBaseValue(float NewValue);

	/** Called when a prediction key that played a montage is rejected */
	void OnCustomPredictiveMontageRejected(UAnimMontage* PredictiveMontage);
	
private:
	TArray<ShieldInfo*> ShieldInfos;
	void AddShieldInfo(FActiveGameplayEffectHandle GEHandle);
	void RemoveShieldInfo(FActiveGameplayEffectHandle GEHandle);
	float GetTotalRemainShield();
	void UpdateTotalRemainShield();
	UFUNCTION()
	void OnApplyGameplayEffectCallback(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
	UFUNCTION(Client, Reliable)
	void OnApplyGameplayEffectCallback_Client(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
	void OnApplyGameplayEffectCallback_Client_Implementation(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
	
	UFUNCTION()
	void OnApplyGameplayEffectToTargetCallback(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
	
	void OnApplyActiveGEToSelfCallback(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
	void OnPeriodicGEExecuteOnSelfCallback(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
	void OnPeriodicGEExecuteOnTargetCallback(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
	void OnImmunityBlockGECallback(const FGameplayEffectSpec& BlockedSpec, const FActiveGameplayEffect* ImmunityGameplayEffect);

	UFUNCTION()
	void RegisterGameplayAttributeValueChangeDelegate(FGameplayAttribute Attribute, const FOnGameplayAttributeValueChangeDelegate& Callback);
	
	UFUNCTION()
	void OnGameplayEffectRemovedCallback(const FActiveGameplayEffect& AGE);

	UFUNCTION()
	void OnAbilityEndedCallback(const FAbilityEndedData& AbilityData);
	
	virtual FActiveGameplayEffectHandle ApplyGameplayEffectSpecToSelf(const FGameplayEffectSpec& GameplayEffect, FPredictionKey PredictionKey = FPredictionKey()) override;
protected:
	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;

public:
	UFUNCTION()
	void SetAttributeValue(FGameplayAttribute Attribute, float NewValue);
	UFUNCTION()
	void K2_SetReplicationMode(EGameplayEffectReplicationMode NewReplicationMode);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	bool bAbilitiesGiven = false;
	bool bStartupEffectsApplied = false;
	
	FAnimNotifyStateDelegate AnimNotifyStateDelegate;
	FAnimNotifyDelegate AnimNotifyDelegate;

	UPROPERTY()
	FDynamicAbilityDelegate OnAbilityCommit;
	
	UPROPERTY()
	FAbilityEndDelegate OnAbilityEnd;

	/** called only on server when GE apply to self. This includes instant and duration based GEs. */
	UPROPERTY()
	FOnGameplayEffectAppliedDynamicDelegate OnGameplayEffectApplied;
	
	/** Called on server whenever a GE is applied to someone else. This includes instant and duration based GEs. */
	UPROPERTY()
	FOnGameplayEffectAppliedDynamicDelegate OnGEAppliedToTarget;

	/** Called on both client and server whenever a duraton based GE is added (E.g., instant GEs do not trigger this). */
	UPROPERTY()
	FOnGameplayEffectAppliedDynamicDelegate OnActiveGEAddedToSelf;

	/** Called on server whenever a periodic GE executes on self */
	UPROPERTY()
	FOnGameplayEffectAppliedDynamicDelegate OnPeriodicGEExecuteOnSelf;

	/** Called on server whenever a periodic GE executes on target */
	UPROPERTY()
	FOnGameplayEffectAppliedDynamicDelegate OnPeriodicGEExecuteOnTarget;

	/** Immunity notification support */
	UPROPERTY()
	FOnImmunityBlockGE OnImmunityBlockGE;

	/** called only on server when GE Removed from self. */
	UPROPERTY()
	FOnGameplayEffectRemovedDynamicDelegate OnGameplayEffectRemoved;

	/** called both on server and client, Pre GE applied to my AttributeSet*/
	UPROPERTY()
	FGEApplyDelegate PreGEExecuteDelegate;
	
	/** called both on server and client, Post GE applied to my AttributeSet*/
	UPROPERTY()
	FGEApplyDelegate PostGEExecuteEvent;

	UPROPERTY()
	FOnShieldChangeDelegate OnShieldChangeEvent;

	UPROPERTY()
	FAbilityEndedDelegate OnAbilityEndedEvent;
	UPROPERTY()
	FGameplayTagChangeDelegate OnGameplayTagChange;

	UPROPERTY()
	FCustomAbilityFailedDelegate OnCustomAbilityFailed;

	UPROPERTY()
	FOnGameplayEffectAppliedDynamicDelegate OnGameplayEffectApplied_Client;

	/** Global callback that can handle game-specific code that needs to run before applying a gameplay effect spec */
	UPROPERTY()
	FPreGameplayEffectApplyToSelfDelegate PreGameplayEffectApplyToSelfEvent;
	/** Global callback that can handle game-specific code that needs to run before applying a gameplay effect spec */
	UPROPERTY()
	FPreGameplayEffectApplyToTargetDelegate PreGameplayEffectApplyToTargetEvent;

	UPROPERTY(Replicated)
	int32 ReviveOtherCount =0;
};
