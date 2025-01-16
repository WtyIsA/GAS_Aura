
---@class ColorCfg
---@field id number
---@field color string
ColorCfg = FLuaCfgInterface.RegisTable([[{"name":"ColorCfg","fields":[{"name":"id","type":"int"}, {"name":"color","type":"string"}]}]]);

---@param k number
---@return ColorCfg
function ColorCfg:Get(k)
	return ColorCfg[k]
end

--color.xml
--颜色表.txt
