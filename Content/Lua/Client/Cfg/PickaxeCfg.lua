
---@class PickaxeCfg
---@field id number
---@field GCId number
PickaxeCfg = FLuaCfgInterface.RegisTable([[{"name":"PickaxeCfg","fields":[{"name":"id","type":"int"}, {"name":"GCId","type":"int"}]}]]);

---@param k number
---@return PickaxeCfg
function PickaxeCfg:Get(k)
	return PickaxeCfg[k]
end

--mining.xml
--挖矿相关/铁镐表.txt
