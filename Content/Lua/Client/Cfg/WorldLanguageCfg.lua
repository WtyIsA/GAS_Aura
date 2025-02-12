
---@class WorldLanguageCfg
---@field id number
---@field language string
---@field shorthand string
---@field channelId number
WorldLanguageCfg = FLuaCfgInterface.RegisTable([[{"name":"WorldLanguageCfg","fields":[{"name":"id","type":"int"}, {"name":"language","type":"string"}, {"name":"shorthand","type":"string"}, {"name":"channelId","type":"int"}]}]]);

---@param k number
---@return WorldLanguageCfg
function WorldLanguageCfg:Get(k)
	return WorldLanguageCfg[k]
end

--multilanguage.xml
--字符串/全球语种对应表.txt
