
---@class DefaultHighCfg
---@field id number
---@field veryLow number
---@field low number
---@field medium number
---@field high number
---@field veryHigh number
---@field options ArrayList<number>
---@field name string
---@field tip string
DefaultHighCfg = FLuaCfgInterface.RegisTable([[{"name":"DefaultHighCfg","fields":[{"name":"id","type":"int"}, {"name":"veryLow","type":"int"}, {"name":"low","type":"int"}, {"name":"medium","type":"int"}, {"name":"high","type":"int"}, {"name":"veryHigh","type":"int"}, {"name":"options","type":"list","value":"int"}, {"name":"name","type":"string"}, {"name":"tip","type":"string"}]}]]);

---@param k number
---@return DefaultHighCfg
function DefaultHighCfg:Get(k)
	return DefaultHighCfg[k]
end

--set.xml
--设置相关/高级设置默认表.txt
