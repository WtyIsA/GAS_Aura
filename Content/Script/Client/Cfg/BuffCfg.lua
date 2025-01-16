
---@class BuffCfg
---@field id string
---@field isShowTimeDown boolean
---@field alwaysShow boolean
---@field icon string
---@field inactiveIcon string
---@field sort number
BuffCfg = FLuaCfgInterface.RegisTable([[{"name":"BuffCfg","fields":[{"name":"id","type":"string"}, {"name":"isShowTimeDown","type":"bool"}, {"name":"alwaysShow","type":"bool"}, {"name":"icon","type":"string"}, {"name":"inactiveIcon","type":"string"}, {"name":"sort","type":"int"}]}]]);

---@param k string
---@return BuffCfg
function BuffCfg:Get(k)
	return BuffCfg[k]
end

--battle.xml
--技能相关/buff表.txt
