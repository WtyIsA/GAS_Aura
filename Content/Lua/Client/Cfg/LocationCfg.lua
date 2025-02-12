
---@class LocationCfg
---@field id number
---@field x number
---@field y number
---@field z number
---@field roll number
---@field pitch number
---@field rotate number
LocationCfg = FLuaCfgInterface.RegisTable([[{"name":"LocationCfg","fields":[{"name":"id","type":"int"}, {"name":"x","type":"float"}, {"name":"y","type":"float"}, {"name":"z","type":"float"}, {"name":"roll","type":"float"}, {"name":"pitch","type":"float"}, {"name":"rotate","type":"float"}]}]]);

---@param k number
---@return LocationCfg
function LocationCfg:Get(k)
	return LocationCfg[k]
end

--location.xml
--位置表.txt
