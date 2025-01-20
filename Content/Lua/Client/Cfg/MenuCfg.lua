
---@class MenuCfg
---@field id number
---@field name string
---@field icon string
MenuCfg = FLuaCfgInterface.RegisTable([[{"name":"MenuCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"icon","type":"string"}]}]]);

---@param k number
---@return MenuCfg
function MenuCfg:Get(k)
	return MenuCfg[k]
end

--battle.xml
--交互按钮表.txt
