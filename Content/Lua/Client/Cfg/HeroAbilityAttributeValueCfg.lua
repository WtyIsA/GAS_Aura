
---@class HeroAbilityAttributeValueCfg
---@field id number
---@field fixedValue number
---@field percent number
HeroAbilityAttributeValueCfg = FLuaCfgInterface.RegisTable([[{"name":"HeroAbilityAttributeValueCfg","fields":[{"name":"id","type":"int"}, {"name":"fixedValue","type":"float"}, {"name":"percent","type":"float"}]}]]);

---@param k number
---@return HeroAbilityAttributeValueCfg
function HeroAbilityAttributeValueCfg:Get(k)
	return HeroAbilityAttributeValueCfg[k]
end

--hero.xml
--英雄相关/技能伤害属性表.txt
