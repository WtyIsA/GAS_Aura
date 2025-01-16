
---@class AltarBuffCfg
---@field id number
---@field GEIds ArrayList<number>
---@field interactiveTime number
---@field loopAudioId number
---@field endAudioId number
AltarBuffCfg = FLuaCfgInterface.RegisTable([[{"name":"AltarBuffCfg","fields":[{"name":"id","type":"int"}, {"name":"GEIds","type":"list","value":"int"}, {"name":"interactiveTime","type":"float"}, {"name":"loopAudioId","type":"int"}, {"name":"endAudioId","type":"int"}]}]]);

---@param k number
---@return AltarBuffCfg
function AltarBuffCfg:Get(k)
	return AltarBuffCfg[k]
end

--battle.xml
--祭坛相关/祭坛表.txt
