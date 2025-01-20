
---@class MiningCfg
---@field id number
---@field interactiveTime number
MiningCfg = FLuaCfgInterface.RegisTable([[{"name":"MiningCfg","fields":[{"name":"id","type":"int"}, {"name":"interactiveTime","type":"int"}]}]]);

---@param k number
---@return MiningCfg
function MiningCfg:Get(k)
	return MiningCfg[k]
end

--mining.xml
--挖矿相关/挖矿产出表.txt
