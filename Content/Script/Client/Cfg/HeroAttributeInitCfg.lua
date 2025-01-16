
---@class HeroAttributeInitCfg
---@field id number
---@field physicalAttack number
---@field magicalAttack number
---@field maxHealth number
---@field maxMana number
---@field maxAnger number
---@field maxEnergy number
---@field gainManaSpeed number
---@field gainAngerSpeed number
---@field gainEnergySpeed number
---@field Armor number
---@field magicResistance number
---@field moveSpeed number
---@field equipSpeed number
---@field interactionSpeed number
---@field coolingReduction number
---@field attackSpeed number
---@field castSpeed number
---@field heal number
---@field reloadSpeed number
---@field projectileReduce number
---@field meleeReduce number
---@field damageReduce number
---@field finalAttackAddPercent number
---@field extraProvideShieldPercent number
---@field ArmorPenetrate_P number
---@field MagicResPenetrate_P number
---@field DecrHeadDamage number
---@field DecrBodyDamage number
---@field DecrLimbDamage number
---@field IncHeadDamage number
---@field IncBodyDamage number
---@field IncLimbDamage number
---@field PhysicalResistance number
---@field ArcaneResistance number
---@field FireResistance number
---@field IceResistance number
---@field LightningResistance number
---@field HolyResistance number
---@field PoisonResistance number
---@field DarkResistance number
---@field PhyAttackAddition number
---@field ArcaneDamageIncrease number
---@field FireDamageIncrease number
---@field IceDamageIncrease number
---@field LightningDamageIncrease number
---@field HolyDamageIncrease number
---@field PoisonDamageIncrease number
---@field DarkDamageIncrease number
---@field IncEvilMonsDamage number
---@field IncUndeadMonsDamage number
---@field IncBioBeastMonsDamage number
HeroAttributeInitCfg = FLuaCfgInterface.RegisTable([[{"name":"HeroAttributeInitCfg","fields":[{"name":"id","type":"int"}, {"name":"physicalAttack","type":"int"}, {"name":"magicalAttack","type":"int"}, {"name":"maxHealth","type":"int"}, {"name":"maxMana","type":"int"}, {"name":"maxAnger","type":"int"}, {"name":"maxEnergy","type":"int"}, {"name":"gainManaSpeed","type":"float"}, {"name":"gainAngerSpeed","type":"float"}, {"name":"gainEnergySpeed","type":"float"}, {"name":"Armor","type":"int"}, {"name":"magicResistance","type":"int"}, {"name":"moveSpeed","type":"float"}, {"name":"equipSpeed","type":"float"}, {"name":"interactionSpeed","type":"float"}, {"name":"coolingReduction","type":"float"}, {"name":"attackSpeed","type":"float"}, {"name":"castSpeed","type":"float"}, {"name":"heal","type":"float"}, {"name":"reloadSpeed","type":"float"}, {"name":"projectileReduce","type":"float"}, {"name":"meleeReduce","type":"float"}, {"name":"damageReduce","type":"float"}, {"name":"finalAttackAddPercent","type":"float"}, {"name":"extraProvideShieldPercent","type":"float"}, {"name":"ArmorPenetrate_P","type":"float"}, {"name":"MagicResPenetrate_P","type":"float"}, {"name":"DecrHeadDamage","type":"float"}, {"name":"DecrBodyDamage","type":"float"}, {"name":"DecrLimbDamage","type":"float"}, {"name":"IncHeadDamage","type":"float"}, {"name":"IncBodyDamage","type":"float"}, {"name":"IncLimbDamage","type":"float"}, {"name":"PhysicalResistance","type":"float"}, {"name":"ArcaneResistance","type":"float"}, {"name":"FireResistance","type":"float"}, {"name":"IceResistance","type":"float"}, {"name":"LightningResistance","type":"float"}, {"name":"HolyResistance","type":"float"}, {"name":"PoisonResistance","type":"float"}, {"name":"DarkResistance","type":"float"}, {"name":"PhyAttackAddition","type":"float"}, {"name":"ArcaneDamageIncrease","type":"float"}, {"name":"FireDamageIncrease","type":"float"}, {"name":"IceDamageIncrease","type":"float"}, {"name":"LightningDamageIncrease","type":"float"}, {"name":"HolyDamageIncrease","type":"float"}, {"name":"PoisonDamageIncrease","type":"float"}, {"name":"DarkDamageIncrease","type":"float"}, {"name":"IncEvilMonsDamage","type":"float"}, {"name":"IncUndeadMonsDamage","type":"float"}, {"name":"IncBioBeastMonsDamage","type":"float"}]}]]);

---@param k number
---@return HeroAttributeInitCfg
function HeroAttributeInitCfg:Get(k)
	return HeroAttributeInitCfg[k]
end

--hero.xml
--英雄相关/英雄属性表.txt
