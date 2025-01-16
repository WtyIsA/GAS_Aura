
---@class TroopStateTypeCfg
---@field id number
---@field icon string
---@field showText string
TroopStateTypeCfg = FLuaCfgInterface.RegisTable([[{"name":"TroopStateTypeCfg","fields":[{"name":"id","type":"int"}, {"name":"icon","type":"string"}, {"name":"showText","type":"string"}]}]]);

---@param k number
---@return TroopStateTypeCfg
function TroopStateTypeCfg:Get(k)
	return TroopStateTypeCfg[k]
end

--troop.xml
--队伍相关/组队状态类型对应图标表.txt
