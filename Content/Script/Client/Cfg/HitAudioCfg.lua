
---@class HitAudioCfg
---@field id number
---@field physicsList ArrayList<number>
HitAudioCfg = FLuaCfgInterface.RegisTable([[{"name":"HitAudioCfg","fields":[{"name":"id","type":"int"}, {"name":"physicsList","type":"list","value":"int"}]}]]);

---@param k number
---@return HitAudioCfg
function HitAudioCfg:Get(k)
	return HitAudioCfg[k]
end

--battle.xml
--战斗相关/击中音效表.txt
