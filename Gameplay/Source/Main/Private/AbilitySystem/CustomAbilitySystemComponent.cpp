
#include "AbilitySystem/CustomAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "GameplayEffectExtension.h"
#include "Attributes/CombatAttributeSet.h"


// Sets default values for this component's properties
UCustomAbilitySystemComponent::UCustomAbilitySystemComponent()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UCustomAbilitySystemComponent::OnApplyGameplayEffectCallback);
	OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UCustomAbilitySystemComponent::OnGameplayEffectRemovedCallback);
	OnGameplayEffectAppliedDelegateToTarget.AddUObject(this, &UCustomAbilitySystemComponent::OnApplyGameplayEffectToTargetCallback);
	OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UCustomAbilitySystemComponent::OnApplyActiveGEToSelfCallback);
	OnPeriodicGameplayEffectExecuteDelegateOnSelf.AddUObject(this, &UCustomAbilitySystemComponent::OnPeriodicGEExecuteOnSelfCallback);
	OnPeriodicGameplayEffectExecuteDelegateOnTarget.AddUObject(this, &UCustomAbilitySystemComponent::OnPeriodicGEExecuteOnTargetCallback);
	OnImmunityBlockGameplayEffectDelegate.AddUObject(this, &UCustomAbilitySystemComponent::OnImmunityBlockGECallback);

	OnAbilityEnded.AddUObject(this, &UCustomAbilitySystemComponent::OnAbilityEndedCallback);
}

FString UCustomAbilitySystemComponent::GetModuleName_Implementation() const
{
	return FString("Share.GAS.CustomASC");
}

void UCustomAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
	ReceiveBeginPlay();
}

void UCustomAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ReceiveEndPlay(EndPlayReason);
}

float UCustomAbilitySystemComponent::CustomPlayMontage(UGameplayAbility* InAnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* NewAnimMontage, float InPlayRate, FName StartSectionName, float StartTimeSeconds,bool bStopAllMontages /*= true*/)
{
	float Duration = -1.f;

	UAnimInstance* AnimInstance = AbilityActorInfo.IsValid() ? AbilityActorInfo->GetAnimInstance() : nullptr;
	if (AnimInstance && NewAnimMontage)
	{
		Duration = AnimInstance->Montage_Play(NewAnimMontage, InPlayRate, EMontagePlayReturnType::MontageLength, StartTimeSeconds,bStopAllMontages);
		if (Duration > 0.f)
		{
			if (NewAnimMontage->HasRootMotion() && AnimInstance->GetOwningActor())
			{
				UE_LOG(LogRootMotion, Log, TEXT("UAbilitySystemComponent::PlayMontage %s, Role: %s")
					, *GetNameSafe(NewAnimMontage)
					, *UEnum::GetValueAsString(TEXT("Engine.ENetRole"), AnimInstance->GetOwningActor()->GetLocalRole())
					);
			}

			LocalAnimMontageInfo.AnimMontage = NewAnimMontage;
			LocalAnimMontageInfo.AnimatingAbility = InAnimatingAbility;
			LocalAnimMontageInfo.PlayBit = !LocalAnimMontageInfo.PlayBit;
			
			if (InAnimatingAbility)
			{
				InAnimatingAbility->SetCurrentMontage(NewAnimMontage);
			}
			
			// Start at a given Section.
			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, NewAnimMontage);
			}

			// Replicate to non owners
			if (IsOwnerActorAuthoritative())
			{
				FGameplayAbilityRepAnimMontage& MutableRepAnimMontageInfo = GetRepAnimMontageInfo_Mutable();

				// Those are static parameters, they are only set when the montage is played. They are not changed after that.
				MutableRepAnimMontageInfo.AnimMontage = NewAnimMontage;
				MutableRepAnimMontageInfo.ForcePlayBit = !bool(MutableRepAnimMontageInfo.ForcePlayBit);

				MutableRepAnimMontageInfo.SectionIdToPlay = 0;
				if (MutableRepAnimMontageInfo.AnimMontage && StartSectionName != NAME_None)
				{
					// we add one so INDEX_NONE can be used in the on rep
					MutableRepAnimMontageInfo.SectionIdToPlay = MutableRepAnimMontageInfo.AnimMontage->GetSectionIndex(StartSectionName) + 1;
				}

				// Update parameters that change during Montage life time.
				AnimMontage_UpdateReplicatedData();

				// Force net update on our avatar actor
				if (AbilityActorInfo->AvatarActor != nullptr)
				{
					AbilityActorInfo->AvatarActor->ForceNetUpdate();
				}
			}
			else
			{
				// If this prediction key is rejected, we need to end the preview
				FPredictionKey PredictionKey = GetPredictionKeyForNewAction();
				if (PredictionKey.IsValidKey())
				{
					PredictionKey.NewRejectedDelegate().BindUObject(this, &UCustomAbilitySystemComponent::OnCustomPredictiveMontageRejected, NewAnimMontage);
				}
			}
		}
	}

	return Duration;
}

void UCustomAbilitySystemComponent::RemoveAllActiveGE()
{
	for (const FActiveGameplayEffectHandle& EffectHandle : ActiveGameplayEffects.GetAllActiveEffectHandles())
	{
		RemoveActiveGameplayEffect(EffectHandle);
	}
}

void UCustomAbilitySystemComponent::NotifyAbilityCommit(UGameplayAbility* Ability)
{
	Super::NotifyAbilityCommit(Ability);
	OnAbilityCommit.Broadcast(Ability);
}

void UCustomAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability,
                                                       bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
	OnAbilityEnd.Broadcast(bWasCancelled, Handle,Ability);
}

void UCustomAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	OnCustomAbilityFailed.Broadcast(Handle, FailureReason);
}

void UCustomAbilitySystemComponent::ExecuteGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters & GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetAvatarActor(), GameplayCueTag, EGameplayCueEvent::Type::Executed, GameplayCueParameters);
}

void UCustomAbilitySystemComponent::AddGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters & GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetAvatarActor(), GameplayCueTag, EGameplayCueEvent::Type::OnActive, GameplayCueParameters);
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetAvatarActor(), GameplayCueTag, EGameplayCueEvent::Type::WhileActive, GameplayCueParameters);
}

void UCustomAbilitySystemComponent::RemoveGameplayCueLocal(const FGameplayTag GameplayCueTag, const FGameplayCueParameters & GameplayCueParameters)
{
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(GetAvatarActor(), GameplayCueTag, EGameplayCueEvent::Type::Removed, GameplayCueParameters);
}

void UCustomAbilitySystemComponent::K2_ExecuteGameplayCue(const FGameplayTag GameplayCueTag,
                                                          FGameplayEffectContextHandle EffectContext)
{
	ExecuteGameplayCue(GameplayCueTag, EffectContext);
}

void UCustomAbilitySystemComponent::K2_ExecuteGameplayCue_WithParams(const FGameplayTag GameplayCueTag,
                                                                     const FGameplayCueParameters&
                                                                     GameplayCueParameters)
{
	ExecuteGameplayCue(GameplayCueTag, GameplayCueParameters);
}

void UCustomAbilitySystemComponent::K2_AddGameplayCue(const FGameplayTag GameplayCueTag,
                                                      FGameplayEffectContextHandle EffectContext)
{
	AddGameplayCue(GameplayCueTag, EffectContext);
}

void UCustomAbilitySystemComponent::K2_AddGameplayCue_WithParams(const FGameplayTag GameplayCueTag,
                                                                 const FGameplayCueParameters& GameplayCueParameters)
{
	AddGameplayCue(GameplayCueTag, GameplayCueParameters);
}

void UCustomAbilitySystemComponent::K2_RemoveGameplayCue(const FGameplayTag GameplayCueTag, bool isPrediction, int32 id)
{
	if (isPrediction)
	{
		FScopedPredictionWindow ScopedPredictionWindow(this, true);
	}
	RemoveGameplayCue(GameplayCueTag, id);
}

void UCustomAbilitySystemComponent::RegisterGameplayTagChangeEvent(const FGameplayTag& Tag,	const FOnTagCountChanged& Callback, EGameplayTagEventType::Type EventType)
{
	RegisterGameplayTagEvent(Tag, EventType).AddLambda([Callback](const FGameplayTag& Tag, int32 Count)
	{
		Callback.ExecuteIfBound(Tag, Count);
	});
}

void UCustomAbilitySystemComponent::UnRegisterGameplayTagChangeEvent(const FGameplayTag& Tag, EGameplayTagEventType::Type EventType)
{
	GameplayTagCountContainer.RegisterGameplayTagEvent(Tag, EventType).Clear();
}

void UCustomAbilitySystemComponent::K2_AddLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count)
{
	AddLooseGameplayTag(GameplayTag, Count);
}

void UCustomAbilitySystemComponent::K2_RemoveLooseGameplayTag(const FGameplayTag& GameplayTag, int32 Count)
{
	RemoveLooseGameplayTag(GameplayTag, Count);
}

TArray<ShieldInfo*>& UCustomAbilitySystemComponent::GetShieldInfos()
{
	return ShieldInfos;
}

void UCustomAbilitySystemComponent::AddShieldInfo(FActiveGameplayEffectHandle GEHandle)
{
	const FActiveGameplayEffect* ActiveGameplayEffect = GetActiveGameplayEffect(GEHandle);
	if (!ActiveGameplayEffect)
	{
		return;
	}
	for (const auto Info : ShieldInfos)
	{
		if (Info->GEHandle == GEHandle)
		{
			if (ActiveGameplayEffect->Spec.Def->Modifiers[0].ModifierOp == EGameplayModOp::Override)
			{
				Info->RemainValue = ActiveGameplayEffect->Spec.Modifiers[0].GetEvaluatedMagnitude();
			}
			else if (ActiveGameplayEffect->Spec.Def->Modifiers[0].ModifierOp == EGameplayModOp::Additive)
			{
				Info->RemainValue += ActiveGameplayEffect->Spec.Modifiers[0].GetEvaluatedMagnitude();
			}
			else
			{
				ABILITY_LOG(Error,TEXT("ModifierOp is not Override or Additive.GE name is %s."), *ActiveGameplayEffect->Spec.Def->GetName())
				return;
			}
			UpdateTotalRemainShield();
			return;
		}
	}
	FGameplayEffectSpec Spec = ActiveGameplayEffect->Spec;
	FGameplayTagContainer Container;
	Spec.GetAllAssetTags(Container);
	if (Container.HasTag(FGameplayTag::RequestGameplayTag(FName(TEXT("GameplayEffect.AddShield")))))
	{
		const float Shield = Spec.Modifiers[0].GetEvaluatedMagnitude();
		if (Shield > 0)
		{
			ShieldInfo* Info = new ShieldInfo();
			Info->GEHandle = GEHandle;
			Info->RemainValue = Shield;
			ShieldInfos.AddUnique(Info);
			UpdateTotalRemainShield();
		}
	}
}

void UCustomAbilitySystemComponent::RemoveShieldInfo(FActiveGameplayEffectHandle GEHandle)
{
	ShieldInfo* CurInfo = nullptr;
	for (const auto Info : ShieldInfos)
	{
		if (Info->GEHandle == GEHandle)
		{
			CurInfo = Info;
		}
	}
	if (CurInfo != nullptr)
	{
		ShieldInfos.Remove(CurInfo);
		UpdateTotalRemainShield();
	}
}

float UCustomAbilitySystemComponent::GetTotalRemainShield()
{
	float TotalRemainShield = 0;
	for (const auto Info : ShieldInfos)
	{
		TotalRemainShield += Info->RemainValue;
	}
	return TotalRemainShield;
}

void UCustomAbilitySystemComponent::UpdateTotalRemainShield()
{
	const UCombatAttributeSet* CombatAttributeSet = Cast<UCombatAttributeSet>(
		GetAttributeSubobject(UCombatAttributeSet::StaticClass()));
	if (CombatAttributeSet)
	{
		SetNumericAttributeBase(CombatAttributeSet->GetRemainShieldAttribute(), GetTotalRemainShield());
	}
}

float UCustomAbilitySystemComponent::GetRemainDamageWithShieldValue(float Damage, const FGameplayEffectSpec& Spec)
{
	TArray<ShieldInfo*> NeedRemove;
	float RemainDamage = Damage;
	for (auto Info : ShieldInfos)
	{
		const float PreValue = Info->RemainValue;
		RemainDamage -= Info->RemainValue;
		Info->RemainValue -= Damage;
		if (Info->RemainValue <= 0)
		{
			Info->RemainValue = 0;
			NeedRemove.Add(Info);
		}
		OnShieldChangeEvent.Broadcast(Info->GEHandle, Spec, PreValue, Info->RemainValue);
		if (RemainDamage <= 0)
		{
			RemainDamage = 0;
			break;
		}
	}
	for (auto ShieldInfo : NeedRemove)
	{
		ShieldInfos.Remove(ShieldInfo);
		RemoveActiveGameplayEffect(ShieldInfo->GEHandle);
	}
	UpdateTotalRemainShield();
	return RemainDamage;
}

void UCustomAbilitySystemComponent::SetMoveSpeedBaseValue(float NewValue)
{
	const UCombatAttributeSet* CombatAttributeSet = Cast<UCombatAttributeSet>(
		GetAttributeSubobject(UCombatAttributeSet::StaticClass()));
	if (CombatAttributeSet)
	{
		SetNumericAttributeBase(CombatAttributeSet->GetMoveSpeedAttribute(), NewValue);
	}
}

void UCustomAbilitySystemComponent::OnCustomPredictiveMontageRejected(UAnimMontage* PredictiveMontage)
{
	static const float MONTAGE_PREDICTION_REJECT_FADETIME = 0.25f;

	UAnimInstance* AnimInstance = AbilityActorInfo.IsValid() ? AbilityActorInfo->GetAnimInstance() : nullptr;
	if (AnimInstance && PredictiveMontage)
	{
		// If this montage is still playing: kill it
		if (AnimInstance->Montage_IsPlaying(PredictiveMontage))
		{
			AnimInstance->Montage_Stop(MONTAGE_PREDICTION_REJECT_FADETIME, PredictiveMontage);
		}
	}
}

void UCustomAbilitySystemComponent::OnApplyGameplayEffectCallback(UAbilitySystemComponent* Source,
                                                                  const FGameplayEffectSpec& SpecApplied,
                                                                  FActiveGameplayEffectHandle ActiveHandle)
{
	OnGameplayEffectApplied.Broadcast(Source, SpecApplied, ActiveHandle);
	OnApplyGameplayEffectCallback_Client(Source, SpecApplied, ActiveHandle);
	AddShieldInfo(ActiveHandle);
}

void UCustomAbilitySystemComponent::OnApplyGameplayEffectCallback_Client_Implementation(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	OnGameplayEffectApplied_Client.Broadcast(Source, SpecApplied, ActiveHandle);
}

void UCustomAbilitySystemComponent::OnApplyGameplayEffectToTargetCallback(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	OnGEAppliedToTarget.Broadcast(Target, SpecApplied, ActiveHandle);
}

void UCustomAbilitySystemComponent::OnApplyActiveGEToSelfCallback(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	OnActiveGEAddedToSelf.Broadcast(Source, SpecApplied, ActiveHandle);
}

void UCustomAbilitySystemComponent::OnPeriodicGEExecuteOnSelfCallback(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	OnPeriodicGEExecuteOnSelf.Broadcast(Source, SpecApplied, ActiveHandle);
}

void UCustomAbilitySystemComponent::OnPeriodicGEExecuteOnTargetCallback(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	OnPeriodicGEExecuteOnTarget.Broadcast(Source, SpecApplied, ActiveHandle);
}

void UCustomAbilitySystemComponent::OnImmunityBlockGECallback(const FGameplayEffectSpec& BlockedSpec, const FActiveGameplayEffect* ImmunityGameplayEffect)
{
	OnImmunityBlockGE.Broadcast(BlockedSpec, *ImmunityGameplayEffect);
}

void UCustomAbilitySystemComponent::RegisterGameplayAttributeValueChangeDelegate(FGameplayAttribute Attribute, const FOnGameplayAttributeValueChangeDelegate& Callback)
{
	GetGameplayAttributeValueChangeDelegate(Attribute).AddLambda([Callback](const FOnAttributeChangeData& Data)
	{
		Callback.ExecuteIfBound(Data.OldValue, Data.NewValue);
	});
}

void UCustomAbilitySystemComponent::CancelAbilityByClass(UGameplayAbility* Ability)
{
	CancelAbility(Ability);
}

void UCustomAbilitySystemComponent::K2_RemoteEndOrCancelAbility(FGameplayAbilitySpecHandle AbilityToEnd,
	FGameplayAbilityActivationInfo ActivationInfo, bool bWasCanceled)
{
	RemoteEndOrCancelAbility(AbilityToEnd,ActivationInfo,bWasCanceled);
}

void UCustomAbilitySystemComponent::K2_CancelAllAbilities(UGameplayAbility* Ignore)
{
	CancelAllAbilities(Ignore);
}

FGameplayAbilitySpecHandle UCustomAbilitySystemComponent::K2_GiveAbility(TSubclassOf<UGameplayAbility> Ability,
                                                                         int32 AbilityID,int32 Lv, int32 InputID)
{
	return GiveAbility(FGameplayAbilitySpec(Ability, AbilityID,Lv, InputID));
}

FGameplayAbilitySpec UCustomAbilitySystemComponent::K2_FindAbilitySpecFromHandle(FGameplayAbilitySpecHandle Handle)
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
	if (Spec)
	{
		return *FindAbilitySpecFromHandle(Handle);
	}
	return nullptr;
}

bool UCustomAbilitySystemComponent::K2_TryActivateAbilityBySpecHandle(FGameplayAbilitySpecHandle AbilityToActivate,
	bool bAllowRemoteActivation)
{
	return TryActivateAbility(AbilityToActivate,bAllowRemoteActivation);
}

void UCustomAbilitySystemComponent::K2_RemoveAbility(const FGameplayAbilitySpecHandle& Handle)
{
	ClearAbility(Handle);
}

const UGameplayEffect* UCustomAbilitySystemComponent::K2_GetGameplayEffectDefForHandle(FActiveGameplayEffectHandle Handle)
{
	return GetGameplayEffectDefForHandle(Handle);
}

FGameplayEffectSpec UCustomAbilitySystemComponent::K2_GetActiveGameplayEffectSpec(const FActiveGameplayEffectHandle Handle) const
{
	return GetActiveGameplayEffect(Handle)->Spec;
}

void UCustomAbilitySystemComponent::OnGameplayEffectRemovedCallback(const FActiveGameplayEffect& AGE)
{
	OnGameplayEffectRemoved.Broadcast(AGE);
	RemoveShieldInfo(AGE.Handle);
	
}

void UCustomAbilitySystemComponent::PreGEExecute(UAbilitySystemComponent& TargetASC, const FGameplayEffectSpec& EffectSpec, FGameplayModifierEvaluatedData& EvaluatedData, float& ToApplyDamage)
{
	PreGEExecuteDelegate.Broadcast(&TargetASC, EffectSpec, EvaluatedData, ToApplyDamage);
}

void UCustomAbilitySystemComponent::PostGEExecute(UAbilitySystemComponent& TargetASC, const FGameplayEffectSpec& EffectSpec, FGameplayModifierEvaluatedData& EvaluatedData, float AppliedDamage)
{
	if (PostGEExecuteEvent.IsBound())
	{
		PostGEExecuteEvent.Broadcast(&TargetASC, EffectSpec, EvaluatedData, AppliedDamage);
	}
}

bool UCustomAbilitySystemComponent::K2_IsCurShield(FActiveGameplayEffectHandle GEHandle)
{
	for (int i = 0; i < ShieldInfos.Num(); ++i)
	{
		if (i == 0 && ShieldInfos[i]->GEHandle == GEHandle)
		{
			return true;
		}
		return false;
	}
	return false;
}

void UCustomAbilitySystemComponent::OnAbilityEndedCallback(const FAbilityEndedData& AbilityData)
{
	if (OnAbilityEndedEvent.IsBound())
	{
		OnAbilityEndedEvent.Broadcast(AbilityData);
	}
}

FActiveGameplayEffectHandle UCustomAbilitySystemComponent::ApplyGameplayEffectSpecToSelf(
	const FGameplayEffectSpec& GameplayEffect, FPredictionKey PredictionKey)
{
	bool IsBound = false;
	if (PreGameplayEffectApplyToSelfEvent.IsBound())
	{
		IsBound = true;
		PreGameplayEffectApplyToSelfEvent.Broadcast(GameplayEffect);
	}
	
	UCustomAbilitySystemComponent* InstigatorASC = Cast<UCustomAbilitySystemComponent>(
		GameplayEffect.GetContext().GetInstigatorAbilitySystemComponent());
	if (InstigatorASC&&InstigatorASC->PreGameplayEffectApplyToTargetEvent.IsBound())
	{
		IsBound = true;
		InstigatorASC->PreGameplayEffectApplyToTargetEvent.Broadcast(this, GameplayEffect);
	}
	if (IsBound)
	{
		FGameplayEffectSpec GameplayEffectTemp = GameplayEffect;
		GameplayEffectTemp.SetContext(GameplayEffect.GetEffectContext());
		return Super::ApplyGameplayEffectSpecToSelf(GameplayEffectTemp, PredictionKey);
	}
	else
	{
		return Super::ApplyGameplayEffectSpecToSelf(GameplayEffect, PredictionKey);
	}
}

void UCustomAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	if (OnGameplayTagChange.IsBound())
	{
		OnGameplayTagChange.Broadcast(Tag, TagExists);
	}
}

void UCustomAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCustomAbilitySystemComponent,ReviveOtherCount);
}

#pragma region For GM, call on server
void UCustomAbilitySystemComponent::SetAttributeValue(FGameplayAttribute Attribute, float NewValue)
{
	ApplyModToAttribute(Attribute,EGameplayModOp::Override,NewValue);
}

void UCustomAbilitySystemComponent::K2_SetReplicationMode(EGameplayEffectReplicationMode NewReplicationMode)
{
	this->SetReplicationMode(NewReplicationMode);
}

#pragma endregion
