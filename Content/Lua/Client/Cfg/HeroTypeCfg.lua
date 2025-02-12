
---@class HeroTypeCfg
---@field id number
---@field name string
---@field desc string
HeroTypeCfg = FLuaCfgInterface.RegisTable([[{"name":"HeroTypeCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"desc","type":"string"}]}]]);

---@param k number
---@return HeroTypeCfg
function HeroTypeCfg:Get(k)
	return HeroTypeCfg[k]
end

--hero.xml
--英雄相关/英雄定位表.txt
