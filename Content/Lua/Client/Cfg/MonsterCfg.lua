
---@class MonsterCfg
---@field id number
---@field name string
---@field raceType number
---@field rankType number
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
---@field ArmorPenetrate_P number
---@field MagicResPenetrate_P number
---@field DecrHeadDamage number
---@field DecrBodyDamage number
---@field DecrLimbDamage number
---@field IncHeadDamage number
---@field IncBodyDamage number
---@field IncLimbDamage number
---@field rotateSpeed number
---@field bpName string
---@field useRagdoll boolean
MonsterCfg = FLuaCfgInterface.RegisTable([[{"name":"MonsterCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"raceType","type":"int"}, {"name":"rankType","type":"int"}, {"name":"PhyAttackAddition","type":"float"}, {"name":"ArcaneDamageIncrease","type":"float"}, {"name":"FireDamageIncrease","type":"float"}, {"name":"IceDamageIncrease","type":"float"}, {"name":"LightningDamageIncrease","type":"float"}, {"name":"HolyDamageIncrease","type":"float"}, {"name":"PoisonDamageIncrease","type":"float"}, {"name":"DarkDamageIncrease","type":"float"}, {"name":"IncEvilMonsDamage","type":"float"}, {"name":"IncUndeadMonsDamage","type":"float"}, {"name":"IncBioBeastMonsDamage","type":"float"}, {"name":"ArmorPenetrate_P","type":"float"}, {"name":"MagicResPenetrate_P","type":"float"}, {"name":"DecrHeadDamage","type":"float"}, {"name":"DecrBodyDamage","type":"float"}, {"name":"DecrLimbDamage","type":"float"}, {"name":"IncHeadDamage","type":"float"}, {"name":"IncBodyDamage","type":"float"}, {"name":"IncLimbDamage","type":"float"}, {"name":"rotateSpeed","type":"float"}, {"name":"bpName","type":"string"}, {"name":"useRagdoll","type":"bool"}]}]]);

---@param k number
---@return MonsterCfg
function MonsterCfg:Get(k)
	return MonsterCfg[k]
end

--monster.xml
--怪物相关/怪物表.txt
