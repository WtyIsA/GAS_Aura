
---@class DropTypeIconCfg
---@field id number
---@field name string
---@field moneyType number
---@field icon string
---@field greyIcon string
---@field quality number
---@field desc string
DropTypeIconCfg = FLuaCfgInterface.RegisTable([[{"name":"DropTypeIconCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"moneyType","type":"int"}, {"name":"icon","type":"string"}, {"name":"greyIcon","type":"string"}, {"name":"quality","type":"int"}, {"name":"desc","type":"string"}]}]]);

---@param k number
---@return DropTypeIconCfg
function DropTypeIconCfg:Get(k)
	return DropTypeIconCfg[k]
end

--drop.xml
--物品相关/掉落类型对应图标表.txt
