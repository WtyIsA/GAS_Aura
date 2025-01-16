
---@class TreasureCfg
---@field id number
---@field name string
---@field columnNum number
---@field rowNum number
---@field lockStatusInteractiveTime number
---@field openStatusInteractiveTime number
---@field openAudio number
TreasureCfg = FLuaCfgInterface.RegisTable([[{"name":"TreasureCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"columnNum","type":"int"}, {"name":"rowNum","type":"int"}, {"name":"lockStatusInteractiveTime","type":"float"}, {"name":"openStatusInteractiveTime","type":"float"}, {"name":"openAudio","type":"int"}]}]]);

---@param k number
---@return TreasureCfg
function TreasureCfg:Get(k)
	return TreasureCfg[k]
end

--box.xml
--场景物相关/宝箱表.txt
