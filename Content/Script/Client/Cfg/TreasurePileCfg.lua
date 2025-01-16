
---@class TreasurePileCfg
---@field id number
---@field time number
TreasurePileCfg = FLuaCfgInterface.RegisTable([[{"name":"TreasurePileCfg","fields":[{"name":"id","type":"int"}, {"name":"time","type":"int"}]}]]);

---@param k number
---@return TreasurePileCfg
function TreasurePileCfg:Get(k)
	return TreasurePileCfg[k]
end

--box.xml
--场景物相关/宝藏堆表.txt
