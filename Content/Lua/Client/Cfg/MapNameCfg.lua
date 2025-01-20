
---@class MapNameCfg
---@field id number
---@field name string
---@field icon string
MapNameCfg = FLuaCfgInterface.RegisTable([[{"name":"MapNameCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"icon","type":"string"}]}]]);

---@param k number
---@return MapNameCfg
function MapNameCfg:Get(k)
	return MapNameCfg[k]
end

--map.xml
--地图相关/地图名称表.txt
