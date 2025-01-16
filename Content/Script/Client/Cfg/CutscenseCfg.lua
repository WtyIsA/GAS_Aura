
---@class CutscenseCfg
---@field id number
---@field cutscensePath string
CutscenseCfg = FLuaCfgInterface.RegisTable([[{"name":"CutscenseCfg","fields":[{"name":"id","type":"int"}, {"name":"cutscensePath","type":"string"}]}]]);

---@param k number
---@return CutscenseCfg
function CutscenseCfg:Get(k)
	return CutscenseCfg[k]
end

--cutscense.xml
--过场动画表.txt
