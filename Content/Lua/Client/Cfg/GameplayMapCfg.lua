
---@class GameplayMapCfg
---@field id number
---@field name string
---@field mode number
---@field modeName string
---@field difficulty string
---@field difficultyIcon string
---@field modIcon string
---@field goldId number
---@field goldNum number
---@field minLevel number
---@field maxLevel number
GameplayMapCfg = FLuaCfgInterface.RegisTable([[{"name":"GameplayMapCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"mode","type":"int"}, {"name":"modeName","type":"string"}, {"name":"difficulty","type":"string"}, {"name":"difficultyIcon","type":"string"}, {"name":"modIcon","type":"string"}, {"name":"goldId","type":"int"}, {"name":"goldNum","type":"int"}, {"name":"minLevel","type":"int"}, {"name":"maxLevel","type":"int"}]}]]);

---@param k number
---@return GameplayMapCfg
function GameplayMapCfg:Get(k)
	return GameplayMapCfg[k]
end

--mapMark.xml
--地图相关/玩法地图表.txt
