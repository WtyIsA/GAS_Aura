require ("CostType")
require ("SalesType")

---@class ItemPriceCfg
---@field itemId number
---@field itemPrice HashMap<number, CostType>
ItemPriceCfg = FLuaCfgInterface.RegisTable([[{"name":"ItemPriceCfg","fields":[{"name":"itemId","type":"int"}, {"name":"itemPrice","type":"map","value":"CostType","key":"int"}]}]]);

---@param k number
---@return ItemPriceCfg
function ItemPriceCfg:Get(k)
	return ItemPriceCfg[k]
end

--merchant.xml
--商人相关/物品价格表.txt
