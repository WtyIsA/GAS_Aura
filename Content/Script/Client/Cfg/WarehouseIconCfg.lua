
---@class WarehouseIconCfg
---@field id number
---@field defaultIcon string
---@field floatingIcon string
---@field selectIcon string
---@field name string
WarehouseIconCfg = FLuaCfgInterface.RegisTable([[{"name":"WarehouseIconCfg","fields":[{"name":"id","type":"int"}, {"name":"defaultIcon","type":"string"}, {"name":"floatingIcon","type":"string"}, {"name":"selectIcon","type":"string"}, {"name":"name","type":"string"}]}]]);

---@param k number
---@return WarehouseIconCfg
function WarehouseIconCfg:Get(k)
	return WarehouseIconCfg[k]
end

--warehouse.xml
--仓库相关/仓库图标表.txt
