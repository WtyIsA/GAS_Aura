
---@class HitEffectCfg
---@field id number
---@field physicsList ArrayList<number>
HitEffectCfg = FLuaCfgInterface.RegisTable([[{"name":"HitEffectCfg","fields":[{"name":"id","type":"int"}, {"name":"physicsList","type":"list","value":"int"}]}]]);

---@param k number
---@return HitEffectCfg
function HitEffectCfg:Get(k)
	return HitEffectCfg[k]
end

--battle.xml
--战斗相关/击中特效表.txt
