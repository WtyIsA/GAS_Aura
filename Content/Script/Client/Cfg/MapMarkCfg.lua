
---@class MapMarkCfg
---@field id number
---@field name string
---@field bigIcon string
---@field miniIcon string
---@field compassIcon string
---@field ZOrder number
MapMarkCfg = FLuaCfgInterface.RegisTable([[{"name":"MapMarkCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"bigIcon","type":"string"}, {"name":"miniIcon","type":"string"}, {"name":"compassIcon","type":"string"}, {"name":"ZOrder","type":"int"}]}]]);

---@param k number
---@return MapMarkCfg
function MapMarkCfg:Get(k)
	return MapMarkCfg[k]
end

--mapMark.xml
--地图相关/地图标记表.txt
