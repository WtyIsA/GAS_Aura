
---@class ItemQualityCfg
---@field id number
---@field icon string
---@field longToShortIcon string
---@field tipsIcon string
---@field equipmentIcon string
---@field appearanceIcon string
---@field transactionIcon string
---@field color number
---@field equipUIColor number
---@field headColor string
---@field chestColor string
---@field legColor string
---@field equipModelColor number
---@field showString string
ItemQualityCfg = FLuaCfgInterface.RegisTable([[{"name":"ItemQualityCfg","fields":[{"name":"id","type":"int"}, {"name":"icon","type":"string"}, {"name":"longToShortIcon","type":"string"}, {"name":"tipsIcon","type":"string"}, {"name":"equipmentIcon","type":"string"}, {"name":"appearanceIcon","type":"string"}, {"name":"transactionIcon","type":"string"}, {"name":"color","type":"int"}, {"name":"equipUIColor","type":"int"}, {"name":"headColor","type":"string"}, {"name":"chestColor","type":"string"}, {"name":"legColor","type":"string"}, {"name":"equipModelColor","type":"int"}, {"name":"showString","type":"string"}]}]]);

---@param k number
---@return ItemQualityCfg
function ItemQualityCfg:Get(k)
	return ItemQualityCfg[k]
end

--item.xml
--物品相关/物品品质对应图标表.txt
