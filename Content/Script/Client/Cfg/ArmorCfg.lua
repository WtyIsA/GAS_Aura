require ("ExtraAttributionIdProbability")
require ("ExtraAttributionNumProbability")

---@class ArmorCfg
---@field id number
---@field slot number
---@field armorType number
---@field extraArmorType number
---@field putOnTime number
---@field manModelId number
---@field womanModelId number
---@field strongManModelId number
---@field extraGA ArrayList<number>
---@field baseAttribution number
---@field extraAttributionNumProbability ArrayList<ExtraAttributionNumProbability>
---@field extraAttributionIdProbability ArrayList<ExtraAttributionIdProbability>
ArmorCfg = FLuaCfgInterface.RegisTable([[{"name":"ArmorCfg","fields":[{"name":"id","type":"int"}, {"name":"slot","type":"int"}, {"name":"armorType","type":"int"}, {"name":"extraArmorType","type":"int"}, {"name":"putOnTime","type":"float"}, {"name":"manModelId","type":"int"}, {"name":"womanModelId","type":"int"}, {"name":"strongManModelId","type":"int"}, {"name":"extraGA","type":"list","value":"int"}, {"name":"baseAttribution","type":"int"}, {"name":"extraAttributionNumProbability","type":"list","value":"ExtraAttributionNumProbability"}, {"name":"extraAttributionIdProbability","type":"list","value":"ExtraAttributionIdProbability"}]}]]);

---@param k number
---@return ArmorCfg
function ArmorCfg:Get(k)
	return ArmorCfg[k]
end

--equipment.xml
--装备相关/防具表.txt
