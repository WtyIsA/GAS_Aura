require ("ExtraAttributionIdProbability")
require ("ExtraAttributionNumProbability")

---@class JewelryCfg
---@field id number
---@field slot ArrayList<number>
---@field putOnTime number
---@field extraGA ArrayList<number>
---@field baseAttribution number
---@field extraAttributionNumProbability ArrayList<ExtraAttributionNumProbability>
---@field extraAttributionIdProbability ArrayList<ExtraAttributionIdProbability>
JewelryCfg = FLuaCfgInterface.RegisTable([[{"name":"JewelryCfg","fields":[{"name":"id","type":"int"}, {"name":"slot","type":"list","value":"int"}, {"name":"putOnTime","type":"float"}, {"name":"extraGA","type":"list","value":"int"}, {"name":"baseAttribution","type":"int"}, {"name":"extraAttributionNumProbability","type":"list","value":"ExtraAttributionNumProbability"}, {"name":"extraAttributionIdProbability","type":"list","value":"ExtraAttributionIdProbability"}]}]]);

---@param k number
---@return JewelryCfg
function JewelryCfg:Get(k)
	return JewelryCfg[k]
end

--equipment.xml
--装备相关/饰品表.txt
