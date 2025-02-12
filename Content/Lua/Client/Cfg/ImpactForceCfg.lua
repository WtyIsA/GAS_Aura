
---@class ImpactForceCfg
---@field impact number
---@field pottery number
---@field woodBox number
---@field woodDoor number
---@field stoneDoor number
---@field egg number
---@field treasureBox number
ImpactForceCfg = FLuaCfgInterface.RegisTable([[{"name":"ImpactForceCfg","fields":[{"name":"impact","type":"int"}, {"name":"pottery","type":"int"}, {"name":"woodBox","type":"int"}, {"name":"woodDoor","type":"int"}, {"name":"stoneDoor","type":"int"}, {"name":"egg","type":"int"}, {"name":"treasureBox","type":"int"}]}]]);

---@param k number
---@return ImpactForceCfg
function ImpactForceCfg:Get(k)
	return ImpactForceCfg[k]
end

--battle.xml
--战斗相关/冲击力表.txt
