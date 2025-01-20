
---@class HeroColorCfg
---@field id number
---@field color string
HeroColorCfg = FLuaCfgInterface.RegisTable([[{"name":"HeroColorCfg","fields":[{"name":"id","type":"int"}, {"name":"color","type":"string"}]}]]);

---@param k number
---@return HeroColorCfg
function HeroColorCfg:Get(k)
	return HeroColorCfg[k]
end

--hero.xml
--英雄相关/英雄颜色表.txt
