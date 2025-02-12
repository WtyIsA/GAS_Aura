// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatAttributeSet.h"
#include "LuaManger.h"
#include "SelfPlayerController.h"

UCombatAttributeSet::UCombatAttributeSet()
	: ExtraProvideShieldPercent(0.0f)
	, AllMagicalResistance(0.0f)
	, LengthenHealPercent(0.0f)
	, AllMagicalDamageIncrease(0.0f)
	, MaxMagicAdditive(0.0f)
	, MagicGainSpeedAdditive(0.0f)
	, FinalAttackAddPercent(0.0f)
	, ReloadSpeed(0.0f)
	, MeleeReduce(0.0f)
	, ProjectileReduce(0.0f)
	, GainEnergySpeed(0.0f)
	, GainManaSpeed(0.0f)
	, Energy(0.0f)
	, MaxEnergy(0.0f)
	, CoolingReduction(0.0f)
	, EvilDamageIncrease(0.0f)
	, GhostDamageIncrease(0.0f)
	, CreatureDamageIncrease(0.0f)
	, Heal(0.0f)
	, EquipSpeed(0.0f)
	, InteractionSpeed(0.0f)
	, BehaviorAudioCoefficient(0.0f)
	, HearRangeCoefficient(0.0f)
	, SeeRangeCoefficient(0.0f)
	, Mana(0.0f)
	, MaxMana(0.0f)
	, Anger(0.0f)
	, MaxAnger(0.0f)
	, GainAngerSpeed(0.0f)
	, PhysicalAttack(0.0f)
	, MagicalAttack(0.0f)
	, Armor(0.0f)
	, MagicResistance(0.0f)
	, PhysicalResistance(0.0f)
	, ArcaneResistance(0.0f)
	, FireResistance(0.0f)
	, IceResistance(0.0f)
	, LightningResistance(0.0f)
	, HolyResistance(0.0f)
	, PoisonResistance(0.0f)
	, DarkResistance(0.0f)
	, ArmorPenetration(0.0f)
	, ArmorPenetrationPercent(0.0f)
	, MagicPenetration(0.0f)
	, MagicPenetrationPercent(0.0f)
	, AttackSpeed(0.0f)
	, CastSpeed(0.0f)
	, HeadDamageReducePercent(0.0f)
	, BodyDamageReducePercent(0.0f)
	, ArmsLegsDamageReducePercent(0.0f)
	, ShieldDamageReducePercent(0.0f)
	, HeadDamageIncrease(0.0f)
	, BodyDamageIncrease(0.0f)
	, ArmsLegsDamageIncrease(0.0f)
	, ShieldDamageIncrease(0.0f)
	, PhysicalDamageIncrease(0.0f)
	, ArcaneDamageIncrease(0.0f)
	, FireDamageIncrease(0.0f)
	, IceDamageIncrease(0.0f)
	, LightningDamageIncrease(0.0f)
	, HolyDamageIncrease(0.0f)
	, PoisonDamageIncrease(0.0f)
	, DarkDamageIncrease(0.0f)
	, HealIncreasePercent(0.0f)
	, MoveSpeed(0.0f)
	, PhysicalBloodsucking(0.0f)
	, MagicalBloodsucking(0.0f)
	, PhysicalReboundDamage(0.0f)
	, PhysicalReboundDamagePercent(0.0f)
	, MagicalReboundDamage(0.0f)
	, MagicalReboundDamagePercent(0.0f)
	, BuffOvertime(0.0f)
	, DebuffReduceTime(0.0f)
	, HealReduceTime(0.0f)
	, DamageReduce(0.0f)
	, Shield(0.0f)
	, RemainShield(0.0f)
	, ImpactForce(0.0f)
	, Damage(0.0f)
	, Healing(0.0f)
	, Health(0.0f)
	, MaxHealth(0.0f)
{
	bOutOfHealth = false;
}

FString UCombatAttributeSet::GetModuleName_Implementation() const
{
	return FString("Share.GAS.AttributeSet.CombatAttributeSet");
}

void UCombatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//for teammate
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet,Health,COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet,MaxHealth,COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet,Mana,COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet,MaxMana,COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet,Anger,COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet,MaxAnger,COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet,Energy,COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet,MaxEnergy,COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Shield, COND_None, REPNOTIFY_Always)							//护盾效果
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, RemainShield, COND_None, REPNOTIFY_Always)

	//for self show info
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, GainAngerSpeed, COND_OwnerOnly, REPNOTIFY_Always)				//怒气值获取速度
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, PhysicalAttack, COND_OwnerOnly, REPNOTIFY_Always)				//物理攻击
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MagicalAttack, COND_OwnerOnly, REPNOTIFY_Always)				//魔法攻击
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Armor, COND_OwnerOnly, REPNOTIFY_Always)						//护甲
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MagicResistance, COND_OwnerOnly, REPNOTIFY_Always)				//魔抗
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, PhysicalResistance, COND_OwnerOnly, REPNOTIFY_Always)			//物理抗性
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ArcaneResistance, COND_OwnerOnly, REPNOTIFY_Always)				//奥术抗性
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, FireResistance, COND_OwnerOnly, REPNOTIFY_Always)				//火焰抗性
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, IceResistance, COND_OwnerOnly, REPNOTIFY_Always)				//冰霜抗性
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, LightningResistance, COND_OwnerOnly, REPNOTIFY_Always)			//雷电抗性
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, HolyResistance, COND_OwnerOnly, REPNOTIFY_Always)				//神圣抗性
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, PoisonResistance, COND_OwnerOnly, REPNOTIFY_Always)				//毒素抗性
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, DarkResistance, COND_OwnerOnly, REPNOTIFY_Always)				//黑暗抗性
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ArmorPenetration, COND_OwnerOnly, REPNOTIFY_Always)				//护甲穿透
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ArmorPenetrationPercent, COND_OwnerOnly, REPNOTIFY_Always)		//护甲穿透百分比
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MagicPenetration, COND_OwnerOnly, REPNOTIFY_Always)				//法术穿透
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MagicPenetrationPercent, COND_OwnerOnly, REPNOTIFY_Always)		//法术穿透百分比
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, AttackSpeed, COND_OwnerOnly, REPNOTIFY_Always)					//攻击速度
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, CastSpeed, COND_OwnerOnly, REPNOTIFY_Always)					//施法速度
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, HeadDamageReducePercent, COND_OwnerOnly, REPNOTIFY_Always)		//头部伤害减免百分比
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, BodyDamageReducePercent, COND_OwnerOnly, REPNOTIFY_Always)		//躯干伤害减免百分比
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ArmsLegsDamageReducePercent, COND_OwnerOnly, REPNOTIFY_Always)	//四肢伤害减免百分比
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, HeadDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)			//对头部伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, BodyDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)			//对躯干伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ArmsLegsDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)		//对四肢伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, PhysicalDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)		//物理伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ArcaneDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)			//奥术伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, FireDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)			//火焰伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, IceDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)			//冰霜伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, LightningDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)		//雷电伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, HolyDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)			//神圣伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, PoisonDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)			//毒伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, DarkDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)			//黑暗伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, HealIncreasePercent, COND_OwnerOnly, REPNOTIFY_Always)			//治疗效果增加百分比
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MoveSpeed, COND_OwnerOnly, REPNOTIFY_Always)					//移动速度
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, PhysicalBloodsucking, COND_OwnerOnly, REPNOTIFY_Always)			//物理吸血
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MagicalBloodsucking, COND_OwnerOnly, REPNOTIFY_Always)			//法术吸血
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, PhysicalReboundDamage, COND_OwnerOnly, REPNOTIFY_Always)		//反弹物理伤害
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, PhysicalReboundDamagePercent, COND_OwnerOnly, REPNOTIFY_Always)	//反弹物理伤害百分比
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MagicalReboundDamage, COND_OwnerOnly, REPNOTIFY_Always)			//反弹魔法伤害
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MagicalReboundDamagePercent, COND_OwnerOnly, REPNOTIFY_Always)	//反弹魔法伤害百分比
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, BuffOvertime, COND_OwnerOnly, REPNOTIFY_Always)					//正面状态持续时间延长
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, DebuffReduceTime, COND_OwnerOnly, REPNOTIFY_Always)				//负面状态持续时间缩减
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, HealReduceTime, COND_OwnerOnly, REPNOTIFY_Always)				//负面状态持续时间缩减
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, DamageReduce, COND_OwnerOnly, REPNOTIFY_Always)					//最终伤害减免
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, Heal, COND_OwnerOnly, REPNOTIFY_Always)							//治疗效果
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, CoolingReduction, COND_OwnerOnly, REPNOTIFY_Always)				//冷却缩减
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, EvilDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)			//对恶魔怪物伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, GhostDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)			//对亡灵怪物伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, CreatureDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)		//对生物怪物伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, EquipSpeed, COND_OwnerOnly, REPNOTIFY_Always)					//装备速度
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, InteractionSpeed, COND_OwnerOnly, REPNOTIFY_Always)				//交互速度
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, BehaviorAudioCoefficient, COND_OwnerOnly, REPNOTIFY_Always)		//行为声音系数
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, HearRangeCoefficient, COND_OwnerOnly, REPNOTIFY_Always)			//听力范围系数
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, SeeRangeCoefficient, COND_OwnerOnly, REPNOTIFY_Always)			//视力范围系数
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ImpactForce, COND_OwnerOnly, REPNOTIFY_Always)					//冲击力
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, GainEnergySpeed, COND_OwnerOnly, REPNOTIFY_Always)				//能量值获取速度
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, GainManaSpeed, COND_OwnerOnly, REPNOTIFY_Always)				//法力值获取速度

	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ShieldDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)			//对盾牌伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ShieldDamageReducePercent, COND_OwnerOnly, REPNOTIFY_Always)	//盾牌伤害减免百分比，非护盾
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ProjectileReduce, COND_OwnerOnly, REPNOTIFY_Always)				//投射物减伤
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MeleeReduce, COND_OwnerOnly, REPNOTIFY_Always)					//近战减伤
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ReloadSpeed, COND_OwnerOnly, REPNOTIFY_Always)					//换弹速度
	
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, AllMagicalResistance, COND_OwnerOnly, REPNOTIFY_Always)			//全法术抗性
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, LengthenHealPercent, COND_OwnerOnly, REPNOTIFY_Always)			//受治疗状态持续时间加成
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, AllMagicalDamageIncrease, COND_OwnerOnly, REPNOTIFY_Always)		//全法术伤害增加
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MaxMagicAdditive, COND_OwnerOnly, REPNOTIFY_Always)				//最大法力/怒气/能量值
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, MagicGainSpeedAdditive, COND_OwnerOnly, REPNOTIFY_Always)		//法力/怒气/能量值获取速度
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, FinalAttackAddPercent, COND_OwnerOnly, REPNOTIFY_Always)		//最终伤害增加
	
	DOREPLIFETIME_CONDITION_NOTIFY(UCombatAttributeSet, ExtraProvideShieldPercent, COND_OwnerOnly, REPNOTIFY_Always)	//提供护盾时额外提升效果百分比
}

bool UCombatAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
		UAbilitySystemComponent* ASC = EffectContext.GetOriginalInstigatorAbilitySystemComponent();
		AActor* AvatarActor = nullptr;
		if (ASC)
		{
			AvatarActor = ASC->GetAvatarActor();
		}
		Data.EvaluatedData.Magnitude = Data.EvaluatedData.Magnitude - GetTeammateDamageReduction(AvatarActor, Data.EffectSpec.Id, Data.EvaluatedData.Magnitude);
	}
	GetCustomAbilitySystemComponent()->PreGEExecute(Data.Target, Data.EffectSpec, Data.EvaluatedData, Data.EvaluatedData.Magnitude);
	return true;
}

void UCombatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
		UAbilitySystemComponent* ASC = EffectContext.GetOriginalInstigatorAbilitySystemComponent();
		AActor* AvatarActor = nullptr;
		if (ASC)
		{
			AvatarActor = ASC->GetAvatarActor();
		}
		
		float CurrentDamage = GetDamage();

		//cheat start
		static FGameplayTag GT = FGameplayTag::RequestGameplayTag("Gameplay.Cheat.Invincible");
		if (Data.Target.HasMatchingGameplayTag(GT))
		{
			CurrentDamage = 0.f;
		}
		//cheat end
		
		CurrentDamage =GetCustomAbilitySystemComponent()->GetRemainDamageWithShieldValue(CurrentDamage, Data.EffectSpec);
		if (CurrentDamage > 0.f)
		{
			SetHealth(GetHealth() - CurrentDamage);
		}
		SetDamage(0.0f);

		if ((GetHealth() <= 0.0f) && !bOutOfHealth)
		{
			if (OnOutOfHealth.IsBound())
			{
				AActor* Instigator = EffectContext.GetOriginalInstigator();
				AActor* Causer = EffectContext.GetEffectCauser();
				OnOutOfHealth.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude,AvatarActor);
			}
		}
	
		// Check health again in case an event above changed it.
		bOutOfHealth = (GetHealth() <= 0.0f);
	}
	else if(Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		SetHealth(GetHealth() + GetHealing());
		SetHealing(0.0f);
	}
	else if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		if (!bOutOfHealth && (GetHealth() <= 0.0f))
		{
			if (OnOutOfHealth.IsBound())
			{
				const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
				UAbilitySystemComponent* ASC = EffectContext.GetOriginalInstigatorAbilitySystemComponent();
				AActor* AvatarActor = nullptr;
				if (ASC)
				{
					AvatarActor = ASC->GetAvatarActor();
				}
				AActor* Instigator = EffectContext.GetOriginalInstigator();
				AActor* Causer = EffectContext.GetEffectCauser();
				OnOutOfHealth.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude,AvatarActor);
			}
		}
		// Check health again in case an event above changed it.
		bOutOfHealth = (GetHealth() <= 0.0f);
	}

	GetCustomAbilitySystemComponent()->PostGEExecute(Data.Target, Data.EffectSpec, Data.EvaluatedData, Data.EvaluatedData.Magnitude);
}

void UCombatAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (PreAttributeChangeDelegate.IsBound())
	{
		NewValue = PreAttributeChangeDelegate.Execute(Attribute,NewValue);
	}

	ClampAttribute(Attribute, NewValue);

	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}
}

void UCombatAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float& PrevValue, float& NewValue)
{
	Super::PostAttributeChange(Attribute, PrevValue, NewValue);
	if (Attribute == GetMaxHealthAttribute())
	{
		if (GetHealth() <= NewValue)
		{
			SetHealth(GetHealth() / PrevValue * NewValue);
		}
		else
		{
			SetHealth(NewValue);
		}
	}

	if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
	{
		if (ASC->IsOwnerActorAuthoritative())
		{
			if (OnAttributeChange.IsBound())
			{
				OnAttributeChange.Broadcast(Attribute, PrevValue, NewValue);
			}
		}
	}
}

void UCombatAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		// Do not allow health to go negative or above max health.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute() || Attribute == GetMaxAngerAttribute() || Attribute == GetMaxManaAttribute() || Attribute == GetMaxEnergyAttribute())
	{
		// Do not allow max health to drop below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetDamageAttribute())
	{
		// Do not allow damage be negative.
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if (Attribute == GetDamageReduceAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
	else if(Attribute == GetHealingAttribute())
	{
		if (bOutOfHealth)
		{
			NewValue = 0.f;
		}
		else
		{
			// Do not allow healing be negative.
			NewValue = FMath::Max(NewValue, 0.f);
		}
	}
	else if(Attribute == GetCoolingReductionAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f,2.f);
	}
	else if(Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}
	else if(Attribute == GetAngerAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxAnger());
	}
	else if(Attribute == GetEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxEnergy());
	}
}

void UCombatAttributeSet::OnRep_GainEnergySpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, GainEnergySpeed, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetGainEnergySpeedAttribute(), OldValue.GetCurrentValue(), GetGainEnergySpeed());
	}
}

void UCombatAttributeSet::OnRep_GainManaSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, GainManaSpeed, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetGainManaSpeedAttribute(), OldValue.GetCurrentValue(), GetGainManaSpeed());
	}
}

void UCombatAttributeSet::OnRep_Energy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Energy, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetEnergyAttribute(), OldValue.GetCurrentValue(), GetEnergy());
	}
}

void UCombatAttributeSet::OnRep_MaxEnergy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MaxEnergy, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMaxEnergyAttribute(), OldValue.GetCurrentValue(), GetMaxEnergy());
	}
}

void UCombatAttributeSet::OnRep_CoolingReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, CoolingReduction, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetCoolingReductionAttribute(), OldValue.GetCurrentValue(), GetCoolingReduction());
	}
}

void UCombatAttributeSet::OnRep_EvilDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, EvilDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetEvilDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetEvilDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_GhostDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, GhostDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetGhostDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetGhostDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_CreatureDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, CreatureDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetCreatureDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetCreatureDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_Heal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Heal, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetHealAttribute(), OldValue.GetCurrentValue(), GetHeal());
	}
}

void UCombatAttributeSet::OnRep_EquipSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, EquipSpeed, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetEquipSpeedAttribute(), OldValue.GetCurrentValue(), GetEquipSpeed());
	}
}

void UCombatAttributeSet::OnRep_InteractionSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, InteractionSpeed, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetInteractionSpeedAttribute(), OldValue.GetCurrentValue(), GetInteractionSpeed());
	}
}

void UCombatAttributeSet::OnRep_BehaviorAudioCoefficient(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, BehaviorAudioCoefficient, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetBehaviorAudioCoefficientAttribute(), OldValue.GetCurrentValue(), GetBehaviorAudioCoefficient());
	}
}

void UCombatAttributeSet::OnRep_HearRangeCoefficient(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, HearRangeCoefficient, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetHearRangeCoefficientAttribute(), OldValue.GetCurrentValue(), GetHearRangeCoefficient());
	}
}

void UCombatAttributeSet::OnRep_SeeRangeCoefficient(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, SeeRangeCoefficient, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetSeeRangeCoefficientAttribute(), OldValue.GetCurrentValue(), GetSeeRangeCoefficient());
	}
}

void UCombatAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Mana, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetManaAttribute(), OldValue.GetCurrentValue(), GetMana());
	}
}

void UCombatAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MaxMana, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMaxManaAttribute(), OldValue.GetCurrentValue(), GetMaxMana());
	}
}

void UCombatAttributeSet::OnRep_Anger(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Anger, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetAngerAttribute(), OldValue.GetCurrentValue(), GetAnger());
	}
}

void UCombatAttributeSet::OnRep_MaxAnger(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MaxAnger, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMaxAngerAttribute(), OldValue.GetCurrentValue(), GetMaxAnger());
	}
}

void UCombatAttributeSet::OnRep_GainAngerSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, GainAngerSpeed, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetGainAngerSpeedAttribute(), OldValue.GetCurrentValue(), GetGainAngerSpeed());
	}
}

void UCombatAttributeSet::OnRep_PhysicalAttack(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, PhysicalAttack, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetPhysicalAttackAttribute(), OldValue.GetCurrentValue(), GetPhysicalAttack());
	}
}

void UCombatAttributeSet::OnRep_MagicalAttack(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MagicalAttack, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMagicalAttackAttribute(), OldValue.GetCurrentValue(), GetMagicalAttack());
	}
}

void UCombatAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Armor, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetArmorAttribute(), OldValue.GetCurrentValue(), GetArmor());
	}
}

void UCombatAttributeSet::OnRep_MagicResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MagicResistance, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMagicResistanceAttribute(), OldValue.GetCurrentValue(), GetMagicResistance());
	}
}

void UCombatAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, PhysicalResistance, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetPhysicalResistanceAttribute(), OldValue.GetCurrentValue(), GetPhysicalResistance());
	}
}

void UCombatAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ArcaneResistance, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetArcaneResistanceAttribute(), OldValue.GetCurrentValue(), GetArcaneResistance());
	}
}

void UCombatAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, FireResistance, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetFireResistanceAttribute(), OldValue.GetCurrentValue(), GetFireResistance());
	}
}

void UCombatAttributeSet::OnRep_IceResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, IceResistance, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetIceResistanceAttribute(), OldValue.GetCurrentValue(), GetIceResistance());
	}
}

void UCombatAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, LightningResistance, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetLightningResistanceAttribute(), OldValue.GetCurrentValue(), GetLightningResistance());
	}
}

void UCombatAttributeSet::OnRep_HolyResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, HolyResistance, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetHolyResistanceAttribute(), OldValue.GetCurrentValue(), GetHolyResistance());
	}
}

void UCombatAttributeSet::OnRep_PoisonResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, PoisonResistance, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetPoisonResistanceAttribute(), OldValue.GetCurrentValue(), GetPoisonResistance());
	}
}

void UCombatAttributeSet::OnRep_DarkResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, DarkResistance, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetDarkResistanceAttribute(), OldValue.GetCurrentValue(), GetDarkResistance());
	}
}

void UCombatAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ArmorPenetration, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetArmorPenetrationAttribute(), OldValue.GetCurrentValue(), GetArmorPenetration());
	}
}

void UCombatAttributeSet::OnRep_ArmorPenetrationPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ArmorPenetrationPercent, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetArmorPenetrationPercentAttribute(), OldValue.GetCurrentValue(), GetArmorPenetrationPercent());
	}
}

void UCombatAttributeSet::OnRep_MagicPenetration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MagicPenetration, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMagicPenetrationAttribute(), OldValue.GetCurrentValue(), GetMagicPenetration());
	}
}

void UCombatAttributeSet::OnRep_MagicPenetrationPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MagicPenetrationPercent, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMagicPenetrationPercentAttribute(), OldValue.GetCurrentValue(), GetMagicPenetrationPercent());
	}
}

void UCombatAttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, AttackSpeed, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetAttackSpeedAttribute(), OldValue.GetCurrentValue(), GetAttackSpeed());
	}
}

void UCombatAttributeSet::OnRep_CastSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, CastSpeed, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetCastSpeedAttribute(), OldValue.GetCurrentValue(), GetCastSpeed());
	}
}

void UCombatAttributeSet::OnRep_HeadDamageReducePercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, HeadDamageReducePercent, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetHeadDamageReducePercentAttribute(), OldValue.GetCurrentValue(), GetHeadDamageReducePercent());
	}
}

void UCombatAttributeSet::OnRep_BodyDamageReducePercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, BodyDamageReducePercent, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetBodyDamageReducePercentAttribute(), OldValue.GetCurrentValue(), GetBodyDamageReducePercent());
	}
}

void UCombatAttributeSet::OnRep_ArmsLegsDamageReducePercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ArmsLegsDamageReducePercent, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetArmsLegsDamageReducePercentAttribute(), OldValue.GetCurrentValue(), GetArmsLegsDamageReducePercent());
	}
}

void UCombatAttributeSet::OnRep_ShieldDamageReducePercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ShieldDamageReducePercent, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetShieldDamageReducePercentAttribute(), OldValue.GetCurrentValue(), GetShieldDamageReducePercent());
	}
}

void UCombatAttributeSet::OnRep_HeadDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, HeadDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetHeadDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetHeadDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_BodyDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, BodyDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetBodyDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetBodyDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_ArmsLegsDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ArmsLegsDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetArmsLegsDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetArmsLegsDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_ShieldDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ShieldDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetShieldDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetShieldDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_PhysicalDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, PhysicalDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetPhysicalDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetPhysicalDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_ArcaneDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ArcaneDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetArcaneDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetArcaneDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_FireDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, FireDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetFireDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetFireDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_IceDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, IceDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetIceDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetIceDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_LightningDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, LightningDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetLightningDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetLightningDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_HolyDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, HolyDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetHolyDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetHolyDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_PoisonDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, PoisonDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetPoisonDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetPoisonDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_DarkDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, DarkDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetDarkDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetDarkDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_HealIncreasePercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, HealIncreasePercent, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetHealIncreasePercentAttribute(), OldValue.GetCurrentValue(), GetHealIncreasePercent());
	}
}

void UCombatAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MoveSpeed, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMoveSpeedAttribute(), OldValue.GetCurrentValue(), GetMoveSpeed());
	}
}

void UCombatAttributeSet::OnRep_PhysicalBloodsucking(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, PhysicalBloodsucking, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetPhysicalBloodsuckingAttribute(), OldValue.GetCurrentValue(), GetPhysicalBloodsucking());
	}
}

void UCombatAttributeSet::OnRep_MagicalBloodsucking(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MagicalBloodsucking, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMagicalBloodsuckingAttribute(), OldValue.GetCurrentValue(), GetMagicalBloodsucking());
	}
}

void UCombatAttributeSet::OnRep_PhysicalReboundDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, PhysicalReboundDamage, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetPhysicalReboundDamageAttribute(), OldValue.GetCurrentValue(), GetPhysicalReboundDamage());
	}
}

void UCombatAttributeSet::OnRep_PhysicalReboundDamagePercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, PhysicalReboundDamagePercent, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetPhysicalReboundDamagePercentAttribute(), OldValue.GetCurrentValue(), GetPhysicalReboundDamagePercent());
	}
}

void UCombatAttributeSet::OnRep_MagicalReboundDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MagicalReboundDamage, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMagicalReboundDamageAttribute(), OldValue.GetCurrentValue(), GetMagicalReboundDamage());
	}
}

void UCombatAttributeSet::OnRep_MagicalReboundDamagePercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MagicalReboundDamagePercent, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMagicalReboundDamagePercentAttribute(), OldValue.GetCurrentValue(), GetMagicalReboundDamagePercent());
	}
}

void UCombatAttributeSet::OnRep_BuffOvertime(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, BuffOvertime, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetBuffOvertimeAttribute(), OldValue.GetCurrentValue(), GetBuffOvertime());
	}
}

void UCombatAttributeSet::OnRep_DebuffReduceTime(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, DebuffReduceTime, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetDebuffReduceTimeAttribute(), OldValue.GetCurrentValue(), GetDebuffReduceTime());
	}
}

void UCombatAttributeSet::OnRep_HealReduceTime(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, HealReduceTime, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetHealReduceTimeAttribute(), OldValue.GetCurrentValue(), GetHealReduceTime());
	}
}

void UCombatAttributeSet::OnRep_DamageReduce(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, DamageReduce, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetDamageReduceAttribute(), OldValue.GetCurrentValue(), GetDamageReduce());
	}
}

void UCombatAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Shield, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetShieldAttribute(), OldValue.GetCurrentValue(), GetShield());
	}
}

void UCombatAttributeSet::OnRep_RemainShield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, RemainShield, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetRemainShieldAttribute(), OldValue.GetCurrentValue(), GetRemainShield());
	}
}

void UCombatAttributeSet::OnRep_ImpactForce(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ImpactForce, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetImpactForceAttribute(), OldValue.GetCurrentValue(), GetImpactForce());
	}
}

void UCombatAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, Health, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetHealthAttribute(), OldValue.GetCurrentValue(), GetHealth());
	}
}

void UCombatAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MaxHealth, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMaxHealthAttribute(), OldValue.GetCurrentValue(), GetMaxHealth());
	}
}

void UCombatAttributeSet::OnRep_MeleeReduce(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MeleeReduce, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMeleeReduceAttribute(), OldValue.GetCurrentValue(), GetMeleeReduce());
	}
}

void UCombatAttributeSet::OnRep_ProjectileReduce(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ProjectileReduce, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetProjectileReduceAttribute(), OldValue.GetCurrentValue(), GetProjectileReduce());
	}
}

void UCombatAttributeSet::OnRep_ReloadSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ReloadSpeed, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetReloadSpeedAttribute(), OldValue.GetCurrentValue(), GetReloadSpeed());
	}
}

void UCombatAttributeSet::OnRep_AllMagicalResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, AllMagicalResistance, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetAllMagicalResistanceAttribute(), OldValue.GetCurrentValue(), GetAllMagicalResistance());
	}
}

void UCombatAttributeSet::OnRep_LengthenHealPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, LengthenHealPercent, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetLengthenHealPercentAttribute(), OldValue.GetCurrentValue(), GetLengthenHealPercent());
	}
}

void UCombatAttributeSet::OnRep_AllMagicalDamageIncrease(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, AllMagicalDamageIncrease, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetAllMagicalDamageIncreaseAttribute(), OldValue.GetCurrentValue(), GetAllMagicalDamageIncrease());
	}
}

void UCombatAttributeSet::OnRep_MaxMagicAdditive(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MaxMagicAdditive, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMaxMagicAdditiveAttribute(), OldValue.GetCurrentValue(), GetMaxMagicAdditive());
	}
}

void UCombatAttributeSet::OnRep_MagicGainSpeedAdditive(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MaxMagicAdditive, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMaxMagicAdditiveAttribute(), OldValue.GetCurrentValue(), GetMaxMagicAdditive());
	}
}

void UCombatAttributeSet::OnRep_FinalAttackAddPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, MaxMagicAdditive, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetMaxMagicAdditiveAttribute(), OldValue.GetCurrentValue(), GetMaxMagicAdditive());
	}
}

void UCombatAttributeSet::OnRep_ExtraProvideShieldPercent(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCombatAttributeSet, ExtraProvideShieldPercent, OldValue);

	if (OnAttributeChange.IsBound())
	{
		OnAttributeChange.Broadcast(GetExtraProvideShieldPercentAttribute(), OldValue.GetCurrentValue(), GetExtraProvideShieldPercent());
	}
}
