
---@class GECfg
---@field id number
---@field GE string
---@field params ArrayList<number>
---@field tagList ArrayList<string>
---@field gcList ArrayList<number>
---@field GainType number
GECfg = FLuaCfgInterface.RegisTable([[{"name":"GECfg","fields":[{"name":"id","type":"int"}, {"name":"GE","type":"string"}, {"name":"params","type":"list","value":"float"}, {"name":"tagList","type":"list","value":"string"}, {"name":"gcList","type":"list","value":"int"}, {"name":"GainType","type":"int"}]}]]);

---@param k number
---@return GECfg
function GECfg:Get(k)
	return GECfg[k]
end

--battle.xml
--技能相关/GE效果表.txt
