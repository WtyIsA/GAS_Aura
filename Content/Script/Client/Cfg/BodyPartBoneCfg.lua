
---@class BodyPartBoneCfg
---@field id number
---@field bone ArrayList<string>
BodyPartBoneCfg = FLuaCfgInterface.RegisTable([[{"name":"BodyPartBoneCfg","fields":[{"name":"id","type":"int"}, {"name":"bone","type":"list","value":"string"}]}]]);

---@param k number
---@return BodyPartBoneCfg
function BodyPartBoneCfg:Get(k)
	return BodyPartBoneCfg[k]
end

--battle.xml
--战斗相关/受击部位对应骨骼名称表.txt
