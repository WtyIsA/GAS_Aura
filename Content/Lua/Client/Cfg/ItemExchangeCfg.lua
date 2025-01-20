require ("ItemExchangeConsume")

---@class ItemExchangeCfg
---@field itemId number
---@field exchangeItemIds ArrayList<ItemExchangeConsume>
ItemExchangeCfg = FLuaCfgInterface.RegisTable([[{"name":"ItemExchangeCfg","fields":[{"name":"itemId","type":"int"}, {"name":"exchangeItemIds","type":"list","value":"ItemExchangeConsume"}]}]]);

---@param k number
---@return ItemExchangeCfg
function ItemExchangeCfg:Get(k)
	return ItemExchangeCfg[k]
end

--merchant.xml
--商人相关/物品交换表.txt
