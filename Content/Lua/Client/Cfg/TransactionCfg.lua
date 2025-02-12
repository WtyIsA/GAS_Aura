
---@class TransactionCfg
---@field id number
---@field belongType number
---@field name string
---@field rank number
TransactionCfg = FLuaCfgInterface.RegisTable([[{"name":"TransactionCfg","fields":[{"name":"id","type":"int"}, {"name":"belongType","type":"int"}, {"name":"name","type":"string"}, {"name":"rank","type":"int"}]}]]);

---@param k number
---@return TransactionCfg
function TransactionCfg:Get(k)
	return TransactionCfg[k]
end

--transaction.xml
--交易行相关/交易行筛选类型显示名称表.txt
