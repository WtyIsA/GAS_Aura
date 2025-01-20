
---@class AttributeShowNameCfg
---@field id string
---@field attr string
---@field showName string
---@field showPencentName string
---@field showLocation number
---@field showPriority number
---@field isShowPercent boolean
AttributeShowNameCfg = FLuaCfgInterface.RegisTable([[{"name":"AttributeShowNameCfg","fields":[{"name":"id","type":"string"}, {"name":"attr","type":"string"}, {"name":"showName","type":"string"}, {"name":"showPencentName","type":"string"}, {"name":"showLocation","type":"int"}, {"name":"showPriority","type":"int"}, {"name":"isShowPercent","type":"bool"}]}]]);

---@param k string
---@return AttributeShowNameCfg
function AttributeShowNameCfg:Get(k)
	return AttributeShowNameCfg[k]
end

--weapon.xml
--装备相关/属性显示名称表.txt
