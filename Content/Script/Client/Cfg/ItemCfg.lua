
---@class ItemCfg
---@field id number
---@field name string
---@field type number
---@field childType number
---@field programType number
---@field acquisitionType number
---@field tradingType number
---@field tradingChildType number
---@field quality number
---@field isInAccountBag boolean
---@field isInBag boolean
---@field isInBs boolean
---@field columnNum number
---@field rowNum number
---@field maxPileNum number
---@field itemValue number
---@field isDisplayValue boolean
---@field icon string
---@field currencyIcon string
---@field currencyGreyIcon string
---@field longToShortIcon string
---@field desc string
---@field tipsDesc string
---@field transactionSort number
---@field canUse boolean
---@field useImmediately boolean
---@field useEffectDesc string
---@field dropId number
---@field existTime number
---@field groundModelPath string
ItemCfg = FLuaCfgInterface.RegisTable([[{"name":"ItemCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"type","type":"int"}, {"name":"childType","type":"int"}, {"name":"programType","type":"int"}, {"name":"acquisitionType","type":"int"}, {"name":"tradingType","type":"int"}, {"name":"tradingChildType","type":"int"}, {"name":"quality","type":"int"}, {"name":"isInAccountBag","type":"bool"}, {"name":"isInBag","type":"bool"}, {"name":"isInBs","type":"bool"}, {"name":"columnNum","type":"int"}, {"name":"rowNum","type":"int"}, {"name":"maxPileNum","type":"int"}, {"name":"itemValue","type":"int"}, {"name":"isDisplayValue","type":"bool"}, {"name":"icon","type":"string"}, {"name":"currencyIcon","type":"string"}, {"name":"currencyGreyIcon","type":"string"}, {"name":"longToShortIcon","type":"string"}, {"name":"desc","type":"string"}, {"name":"tipsDesc","type":"string"}, {"name":"transactionSort","type":"int"}, {"name":"canUse","type":"bool"}, {"name":"useImmediately","type":"bool"}, {"name":"useEffectDesc","type":"string"}, {"name":"dropId","type":"int"}, {"name":"existTime","type":"int"}, {"name":"groundModelPath","type":"string"}]}]]);

---@param k number
---@return ItemCfg
function ItemCfg:Get(k)
	return ItemCfg[k]
end

--item.xml
--物品相关/物品表.txt
