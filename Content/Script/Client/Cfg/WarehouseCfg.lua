
---@class WarehouseCfg
---@field id number
---@field iconId number
---@field itemId number
---@field itemNum number
WarehouseCfg = FLuaCfgInterface.RegisTable([[{"name":"WarehouseCfg","fields":[{"name":"id","type":"int"}, {"name":"iconId","type":"int"}, {"name":"itemId","type":"int"}, {"name":"itemNum","type":"int"}]}]]);

---@param k number
---@return WarehouseCfg
function WarehouseCfg:Get(k)
	return WarehouseCfg[k]
end

--warehouse.xml
--仓库相关/仓库配置表.txt
