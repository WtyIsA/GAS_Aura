
---@class HeroLevelCfg
---@field id number
---@field needExp number
HeroLevelCfg = FLuaCfgInterface.RegisTable([[{"name":"HeroLevelCfg","fields":[{"name":"id","type":"int"}, {"name":"needExp","type":"int"}]}]]);

---@param k number
---@return HeroLevelCfg
function HeroLevelCfg:Get(k)
	return HeroLevelCfg[k]
end

--hero.xml
--英雄相关/英雄升级表.txt
