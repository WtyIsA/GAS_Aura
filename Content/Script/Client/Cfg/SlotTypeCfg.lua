
---@class SlotTypeCfg
---@field id number
---@field icon string
---@field name string
SlotTypeCfg = FLuaCfgInterface.RegisTable([[{"name":"SlotTypeCfg","fields":[{"name":"id","type":"int"}, {"name":"icon","type":"string"}, {"name":"name","type":"string"}]}]]);

---@param k number
---@return SlotTypeCfg
function SlotTypeCfg:Get(k)
	return SlotTypeCfg[k]
end

--equipment.xml
--装备相关/插槽类型对应图标表.txt
