
---@class WarpGateCfg
---@field id number
---@field name string
---@field bpPath string
---@field effectsPath string
---@field audio number
WarpGateCfg = FLuaCfgInterface.RegisTable([[{"name":"WarpGateCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"bpPath","type":"string"}, {"name":"effectsPath","type":"string"}, {"name":"audio","type":"int"}]}]]);

---@param k number
---@return WarpGateCfg
function WarpGateCfg:Get(k)
	return WarpGateCfg[k]
end

--map.xml
--地图相关/传送门表.txt
