require ("SkillParams")

---@class CharacterAbilityCfg
---@field id number
---@field name string
---@field blueprint string
---@field skillIcon string
---@field activatedSkillIcon string
---@field pressAutoActive boolean
---@field releaseNeedWeaponSubType ArrayList<number>
---@field equipWeaponSubType ArrayList<number>
---@field isShowProgressBar boolean
---@field abilityType number
---@field abilityAttributeType number
---@field animRateAttributeType string
---@field abilityAttributeValues ArrayList<number>
---@field abilityExtraAttributeValues ArrayList<number>
---@field LVSkillParams ArrayList<SkillParams>
---@field cost number
---@field cooldown number
---@field AudioIdList ArrayList<number>
---@field canCancel boolean
---@field skillInfo string
---@field skillCoolDownInfo string
---@field skillCostInfo string
---@field abilityTypeName string
CharacterAbilityCfg = FLuaCfgInterface.RegisTable([[{"name":"CharacterAbilityCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"blueprint","type":"string"}, {"name":"skillIcon","type":"string"}, {"name":"activatedSkillIcon","type":"string"}, {"name":"pressAutoActive","type":"bool"}, {"name":"releaseNeedWeaponSubType","type":"list","value":"int"}, {"name":"equipWeaponSubType","type":"list","value":"int"}, {"name":"isShowProgressBar","type":"bool"}, {"name":"abilityType","type":"int"}, {"name":"abilityAttributeType","type":"int"}, {"name":"animRateAttributeType","type":"string"}, {"name":"abilityAttributeValues","type":"list","value":"int"}, {"name":"abilityExtraAttributeValues","type":"list","value":"int"}, {"name":"LVSkillParams","type":"list","value":"SkillParams"}, {"name":"cost","type":"int"}, {"name":"cooldown","type":"int"}, {"name":"AudioIdList","type":"list","value":"int"}, {"name":"canCancel","type":"bool"}, {"name":"skillInfo","type":"string"}, {"name":"skillCoolDownInfo","type":"string"}, {"name":"skillCostInfo","type":"string"}, {"name":"abilityTypeName","type":"string"}]}]]);

---@param k number
---@return CharacterAbilityCfg
function CharacterAbilityCfg:Get(k)
	return CharacterAbilityCfg[k]
end

--battle.xml
--技能相关/技能表.txt
