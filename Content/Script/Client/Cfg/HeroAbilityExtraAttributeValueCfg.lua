
---@class HeroAbilityExtraAttributeValueCfg
---@field id number
---@field value ArrayList<number>
HeroAbilityExtraAttributeValueCfg = FLuaCfgInterface.RegisTable([[{"name":"HeroAbilityExtraAttributeValueCfg","fields":[{"name":"id","type":"int"}, {"name":"value","type":"list","value":"float"}]}]]);

---@param k number
---@return HeroAbilityExtraAttributeValueCfg
function HeroAbilityExtraAttributeValueCfg:Get(k)
	return HeroAbilityExtraAttributeValueCfg[k]
end

--hero.xml
--英雄相关/技能附加属性表.txt
