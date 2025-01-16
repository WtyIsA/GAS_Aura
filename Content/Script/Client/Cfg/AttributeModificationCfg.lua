require ("AttributeGEList")

---@class AttributeModificationCfg
---@field id number
---@field attributes ArrayList<AttributeGEList>
AttributeModificationCfg = FLuaCfgInterface.RegisTable([[{"name":"AttributeModificationCfg","fields":[{"name":"id","type":"int"}, {"name":"attributes","type":"list","value":"AttributeGEList"}]}]]);

---@param k number
---@return AttributeModificationCfg
function AttributeModificationCfg:Get(k)
	return AttributeModificationCfg[k]
end

--weapon.xml
--装备相关/属性修改表.txt
