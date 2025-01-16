
---@class LevelMapCfg
---@field id number
---@field icon string
---@field mapSize number
---@field maxScale number
---@field lineSize number
---@field x number
---@field y number
---@field z number
---@field mapPath string
---@field refreshBlueprint string
---@field poisonCircleIds ArrayList<number>
LevelMapCfg = FLuaCfgInterface.RegisTable([[{"name":"LevelMapCfg","fields":[{"name":"id","type":"int"}, {"name":"icon","type":"string"}, {"name":"mapSize","type":"int"}, {"name":"maxScale","type":"int"}, {"name":"lineSize","type":"int"}, {"name":"x","type":"float"}, {"name":"y","type":"float"}, {"name":"z","type":"float"}, {"name":"mapPath","type":"string"}, {"name":"refreshBlueprint","type":"string"}, {"name":"poisonCircleIds","type":"list","value":"int"}]}]]);

---@param k number
---@return LevelMapCfg
function LevelMapCfg:Get(k)
	return LevelMapCfg[k]
end

--mapMark.xml
--地图相关/关卡地图表.txt
