
---@class EffectCfg
---@field id number
---@field path string
EffectCfg = FLuaCfgInterface.RegisTable([[{"name":"EffectCfg","fields":[{"name":"id","type":"int"}, {"name":"path","type":"string"}]}]]);

---@param k number
---@return EffectCfg
function EffectCfg:Get(k)
	return EffectCfg[k]
end

--battle.xml
--特效表.txt
