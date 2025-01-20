
---@class TroopUnreadyStateTypeCfg
---@field id number
---@field icon string
---@field showText string
TroopUnreadyStateTypeCfg = FLuaCfgInterface.RegisTable([[{"name":"TroopUnreadyStateTypeCfg","fields":[{"name":"id","type":"int"}, {"name":"icon","type":"string"}, {"name":"showText","type":"string"}]}]]);

---@param k number
---@return TroopUnreadyStateTypeCfg
function TroopUnreadyStateTypeCfg:Get(k)
	return TroopUnreadyStateTypeCfg[k]
end

--troop.xml
--队伍相关/组队未准备类型对应图标表.txt
