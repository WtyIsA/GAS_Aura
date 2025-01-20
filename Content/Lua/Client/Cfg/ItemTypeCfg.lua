
---@class ItemTypeCfg
---@field id number
---@field name string
ItemTypeCfg = FLuaCfgInterface.RegisTable([[{"name":"ItemTypeCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}]}]]);

---@param k number
---@return ItemTypeCfg
function ItemTypeCfg:Get(k)
	return ItemTypeCfg[k]
end

--item.xml
--物品相关/物品类型对应图标表.txt
