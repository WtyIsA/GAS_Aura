// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CustomAttributeSet.h"
#include "AbilitySystem/CustomAbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffectTypes.h"
#include "Net/UnrealNetwork.h"
#include "CombatAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class MAIN_API UCombatAttributeSet : public UCustomAttributeSet,public IUnLuaInterface
{
	GENERATED_BODY()

public:
	UCombatAttributeSet();
	virtual FString GetModuleName_Implementation() const override;
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Health);						//当前生命值
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxHealth);					//最大生命值
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Mana);							//当前法力值
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxMana);						//最大法力值
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Anger);						//当前怒气值
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxAnger);						//最大怒气值
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, GainAngerSpeed);				//怒气值获取速度
	
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, PhysicalAttack);				//物理攻击
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MagicalAttack);				//魔法攻击
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Armor);						//护甲
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MagicResistance);				//魔抗
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, PhysicalResistance);			//物理抗性
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ArcaneResistance);				//奥术抗性
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, FireResistance);				//火焰抗性
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, IceResistance);				//冰霜抗性
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, LightningResistance);			//雷电抗性
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, HolyResistance);				//神圣抗性
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, PoisonResistance);				//毒素抗性
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, DarkResistance);				//黑暗抗性
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ArmorPenetration);				//护甲穿透
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ArmorPenetrationPercent);		//护甲穿透百分比
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MagicPenetration);				//法术穿透
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MagicPenetrationPercent);		//法术穿透百分比
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, AttackSpeed);					//攻击速度
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, CastSpeed);					//施法速度
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, HeadDamageReducePercent);		//头部伤害减免百分比
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, BodyDamageReducePercent);		//躯干伤害减免百分比
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ArmsLegsDamageReducePercent);	//四肢伤害减免百分比
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ShieldDamageReducePercent);	//盾牌伤害减免百分比，非护盾
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, HeadDamageIncrease);			//对头部伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, BodyDamageIncrease);			//对躯干伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ArmsLegsDamageIncrease);		//对四肢伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ShieldDamageIncrease);			//对盾牌伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, PhysicalDamageIncrease);		//物理伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ArcaneDamageIncrease);			//奥术伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, FireDamageIncrease);			//火焰伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, IceDamageIncrease);			//冰霜伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, LightningDamageIncrease);		//雷电伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, HolyDamageIncrease);			//神圣伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, PoisonDamageIncrease);			//毒伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, DarkDamageIncrease);			//黑暗伤害增加
	
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, HealIncreasePercent);			//治疗效果增加百分比

	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MoveSpeed);					//移动速度
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, PhysicalBloodsucking);			//物理吸血
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MagicalBloodsucking);			//法术吸血
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, PhysicalReboundDamage);		//反弹物理伤害
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, PhysicalReboundDamagePercent);	//反弹物理伤害百分比
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MagicalReboundDamage);			//反弹魔法伤害
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MagicalReboundDamagePercent);	//反弹魔法伤害百分比
	
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, BuffOvertime);					//正面状态持续时间延长
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, DebuffReduceTime);				//负面状态持续时间缩减
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, HealReduceTime);				//负面状态持续时间缩减
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, DamageReduce);					//最终伤害减免
	
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Heal);							//治疗效果
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Shield);						//护盾效果
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, RemainShield);					//剩余护盾
	
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Damage);						//伤害 PostGameplayEffectExecute 会立即施加给Health
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Healing);						//治疗 PostGameplayEffectExecute 会立即施加给Health

	// Delegate to broadcast when the health attribute reaches zero.
	mutable FCustomAttributeFourParamsEvent OnOutOfHealth;

	
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, CoolingReduction);				//冷却缩减
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, EvilDamageIncrease);			//对恶魔怪物伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, GhostDamageIncrease);			//对亡灵怪物伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, CreatureDamageIncrease);		//对生物怪物伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, EquipSpeed);					//装备速度
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, InteractionSpeed);				//交互速度
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, BehaviorAudioCoefficient);		//行为声音系数
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, HearRangeCoefficient);			//听力范围系数
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, SeeRangeCoefficient);			//视力范围系数
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ImpactForce);					//冲击力

	
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, Energy);						//当前能量值
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxEnergy);					//最大能量值
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, GainEnergySpeed);				//能量值获取速度
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, GainManaSpeed);				//法力值获取速度
	
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ProjectileReduce);				//投射物减伤
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MeleeReduce);					//近战减伤
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ReloadSpeed);					//换弹速度
	
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, AllMagicalResistance);			//全法术抗性
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, LengthenHealPercent);			//受治疗状态持续时间加成
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, AllMagicalDamageIncrease);		//全法术伤害增加
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MaxMagicAdditive);				//最大法力/怒气/能量值
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, MagicGainSpeedAdditive);		//法力/怒气/能量值获取速度
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, FinalAttackAddPercent);		//最终伤害增加
	
	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, ExtraProvideShieldPercent);	//提供护盾时额外提升效果百分比

private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ExtraProvideShieldPercent, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="提供护盾时额外提升效果百分比")
	FGameplayAttributeData ExtraProvideShieldPercent;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AllMagicalResistance, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="全法术抗性")
	FGameplayAttributeData AllMagicalResistance;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LengthenHealPercent, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="受治疗状态持续时间加成")
	FGameplayAttributeData LengthenHealPercent;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AllMagicalDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="全法术伤害增加")
	FGameplayAttributeData AllMagicalDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMagicAdditive, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="最大法力/怒气/能量值")
	FGameplayAttributeData MaxMagicAdditive;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicGainSpeedAdditive, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="法力/怒气/能量值获取速度")
	FGameplayAttributeData MagicGainSpeedAdditive;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FinalAttackAddPercent, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="最终伤害增加")
	FGameplayAttributeData FinalAttackAddPercent;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ReloadSpeed, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="JinZhanJianShang")
	FGameplayAttributeData ReloadSpeed;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MeleeReduce, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="JinZhanJianShang")
	FGameplayAttributeData MeleeReduce;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ProjectileReduce, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="TouSheWuJianShang")
	FGameplayAttributeData ProjectileReduce;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_GainEnergySpeed, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="NengLiangHuoQuSuDu")
	FGameplayAttributeData GainEnergySpeed;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_GainManaSpeed, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="FaLiHuoQuSuDu")
	FGameplayAttributeData GainManaSpeed;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Energy, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="DangQianNengLiangZhi")
	FGameplayAttributeData Energy;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxEnergy, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ZuiDaNengLiangZhi")
	FGameplayAttributeData MaxEnergy;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CoolingReduction, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="LengQueSuoJian")
	FGameplayAttributeData CoolingReduction;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EvilDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="DuiEMoShangHaiZengJia")
	FGameplayAttributeData EvilDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_GhostDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="DuiWangLingShangHaiZengJia")
	FGameplayAttributeData GhostDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CreatureDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="DuiShengWuShangHaiZengJia")
	FGameplayAttributeData CreatureDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Heal, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ZhiLiaoXiaoGuo")
	FGameplayAttributeData Heal;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EquipSpeed, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ZhuangBeiSuDu")
	FGameplayAttributeData EquipSpeed;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_InteractionSpeed, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="JiaoHuSuDu")
	FGameplayAttributeData InteractionSpeed;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BehaviorAudioCoefficient, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="XingWeiShengYinXiShu")
	FGameplayAttributeData BehaviorAudioCoefficient;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HearRangeCoefficient, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="TingLiFanWeiXiShu")
	FGameplayAttributeData HearRangeCoefficient;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SeeRangeCoefficient, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ShiLiFanWeiXiShu")
	FGameplayAttributeData SeeRangeCoefficient;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="DangQianFaLiZhi")
	FGameplayAttributeData Mana;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ZuiDaFaLiZhi")
	FGameplayAttributeData MaxMana;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Anger, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="DangQianNuQiZhi")
	FGameplayAttributeData Anger;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxAnger, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ZuiDaNuQiZhi")
	FGameplayAttributeData MaxAnger;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_GainAngerSpeed, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="HuoDeNuQiZhi")
	FGameplayAttributeData GainAngerSpeed;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalAttack, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="WuLiGongJi")
	FGameplayAttributeData PhysicalAttack;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicalAttack, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="MoFaGongJi")
	FGameplayAttributeData MagicalAttack;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="HuJia")
	FGameplayAttributeData Armor;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicResistance, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="MoKang")
	FGameplayAttributeData MagicResistance;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalResistance, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="WuLiKangXing")
	FGameplayAttributeData PhysicalResistance;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArcaneResistance, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="AoShuKangXing")
	FGameplayAttributeData ArcaneResistance;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="HuoYanKangXing")
	FGameplayAttributeData FireResistance;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IceResistance, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="BingShuangKangXing")
	FGameplayAttributeData IceResistance;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningResistance, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="LeiDianKangXing")
	FGameplayAttributeData LightningResistance;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HolyResistance, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ShenShengKangXing")
	FGameplayAttributeData HolyResistance;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PoisonResistance, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="DuKangXing")
	FGameplayAttributeData PoisonResistance;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DarkResistance, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="HeiAnKangXing")
	FGameplayAttributeData DarkResistance;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="HuJiaChuanTou")
	FGameplayAttributeData ArmorPenetration;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetrationPercent, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="HuJiaChuanTouBaiFenBi")
	FGameplayAttributeData ArmorPenetrationPercent;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicPenetration, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="FaShuChuanTou")
	FGameplayAttributeData MagicPenetration;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicPenetrationPercent, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="FaShuChuanTouBaiFenBi")
	FGameplayAttributeData MagicPenetrationPercent;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="GongJiSuDu")
	FGameplayAttributeData AttackSpeed;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CastSpeed, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ShiFaSuoJian")
	FGameplayAttributeData CastSpeed;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HeadDamageReducePercent, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="TouBuShangHaiJianMianBaiFenBi")
	FGameplayAttributeData HeadDamageReducePercent;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BodyDamageReducePercent, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="QuGanShangHaiJianMianBaiFenBi")
	FGameplayAttributeData BodyDamageReducePercent;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmsLegsDamageReducePercent, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="SiZhiShangHaiJianMianBaiFenBi")
	FGameplayAttributeData ArmsLegsDamageReducePercent;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ShieldDamageReducePercent, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="DunPaiShangHaiJianMianBaiFenBi")
	FGameplayAttributeData ShieldDamageReducePercent;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HeadDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="DuiTouBuShangHaiZengJia")
	FGameplayAttributeData HeadDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BodyDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="DuiQuGanShangHaiZengJia")
	FGameplayAttributeData BodyDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmsLegsDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="DuiSiZhiShangHaiZengJia")
	FGameplayAttributeData ArmsLegsDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ShieldDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="DuiHuDunShangHaiZengJia")
	FGameplayAttributeData ShieldDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="WuLiShangHaiZengJia")
	FGameplayAttributeData PhysicalDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArcaneDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="AoShuShangHaiZengJia")
	FGameplayAttributeData ArcaneDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="HuoYanShangHaiZengJia")
	FGameplayAttributeData FireDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IceDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="BingShuangShangHaiZengJia")
	FGameplayAttributeData IceDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="LeiDianShangHaiZengJia")
	FGameplayAttributeData LightningDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HolyDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ShenShengShangHaiZengJia")
	FGameplayAttributeData HolyDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PoisonDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="DuShangHaiZengJia")
	FGameplayAttributeData PoisonDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DarkDamageIncrease, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="HeiAnShangHaiZengJia")
	FGameplayAttributeData DarkDamageIncrease;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealIncreasePercent, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ZhiLiaoXiaoGuoZengJiaBaiFenBi")
	FGameplayAttributeData HealIncreasePercent;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="YiDongSuDu")
	FGameplayAttributeData MoveSpeed;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalBloodsucking, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="WuLiXiXue")
	FGameplayAttributeData PhysicalBloodsucking;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicalBloodsucking, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="FaShuXiXue")
	FGameplayAttributeData MagicalBloodsucking;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalReboundDamage, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="FanTanWuLiSHnageHai")
	FGameplayAttributeData PhysicalReboundDamage;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalReboundDamagePercent, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="FanTanWuLiSHnageHaiBaiFenBi")
	FGameplayAttributeData PhysicalReboundDamagePercent;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicalReboundDamage, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="FanTanMoFaShangHai")
	FGameplayAttributeData MagicalReboundDamage;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicalReboundDamagePercent, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="FanTanMoFaShangHaiBaiFenBi")
	FGameplayAttributeData MagicalReboundDamagePercent;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BuffOvertime, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ShouZhengMianZhuangTaiChiXuShiJian")
	FGameplayAttributeData BuffOvertime;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DebuffReduceTime, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ShouFuMianZhuangTaiChiXuShiJianJiaCheng")
	FGameplayAttributeData DebuffReduceTime;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealReduceTime, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ShouZhiLiaoZhuangTaiChiXuShiJianJiaCheng")
	FGameplayAttributeData HealReduceTime;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageReduce, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ZuiZhongShangHaiJianMianJiaCheng")
	FGameplayAttributeData DamageReduce;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Shield, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="HuDunXiaoGuo")
	FGameplayAttributeData Shield;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RemainShield, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="RemainHuDunXiaoGuo")
	FGameplayAttributeData RemainShield;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ImpactForce, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ChongJiLi")
	FGameplayAttributeData ImpactForce;
	
	UPROPERTY(BlueprintReadOnly, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ShangHai")
	FGameplayAttributeData Damage;
	UPROPERTY(BlueprintReadOnly, Category = "Custom|Combat", Meta = (AllowPrivateAccess = true), DisplayName="ZhiLiao")
	FGameplayAttributeData Healing;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	float GetTeammateDamageReduction(AActor* AvatarActor, int GEId, float CurDamage);
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float& PrevValue, float& NewValue) override;;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	// The current health attribute.  The health will be capped by the max health attribute.  Health is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Custom|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current max health attribute.  Max health is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Custom|MaxHealth", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// Used to track when the health reaches 0.
	bool bOutOfHealth;

protected:
	UFUNCTION()
	void OnRep_GainEnergySpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_GainManaSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Energy(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxEnergy(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CoolingReduction(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_EvilDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_GhostDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CreatureDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Heal(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_EquipSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_InteractionSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BehaviorAudioCoefficient(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HearRangeCoefficient(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_SeeRangeCoefficient(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Anger(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxAnger(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_GainAngerSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PhysicalAttack(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MagicalAttack(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MagicResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PhysicalResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ArcaneResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_FireResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_IceResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_LightningResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HolyResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PoisonResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_DarkResistance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ArmorPenetrationPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MagicPenetration(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MagicPenetrationPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CastSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HeadDamageReducePercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BodyDamageReducePercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ArmsLegsDamageReducePercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ShieldDamageReducePercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HeadDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BodyDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ArmsLegsDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ShieldDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PhysicalDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ArcaneDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_FireDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_IceDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_LightningDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HolyDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PoisonDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_DarkDamageIncrease(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HealIncreasePercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PhysicalBloodsucking(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MagicalBloodsucking(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PhysicalReboundDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_PhysicalReboundDamagePercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MagicalReboundDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MagicalReboundDamagePercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BuffOvertime(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_DebuffReduceTime(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_HealReduceTime(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_DamageReduce(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldValue);

	UFUNCTION()
    void OnRep_RemainShield(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ImpactForce(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MeleeReduce(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ProjectileReduce(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ReloadSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AllMagicalResistance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_LengthenHealPercent(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_AllMagicalDamageIncrease(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MaxMagicAdditive(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_MagicGainSpeedAdditive(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	void OnRep_FinalAttackAddPercent(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ExtraProvideShieldPercent(const FGameplayAttributeData& OldValue);
};
