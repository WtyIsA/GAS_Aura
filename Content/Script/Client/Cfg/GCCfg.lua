
---@class GCCfg
---@field id number
---@field tag string
---@field params ArrayList<string>
---@field startAudioList ArrayList<number>
---@field endAudioList ArrayList<number>
GCCfg = FLuaCfgInterface.RegisTable([[{"name":"GCCfg","fields":[{"name":"id","type":"int"}, {"name":"tag","type":"string"}, {"name":"params","type":"list","value":"string"}, {"name":"startAudioList","type":"list","value":"int"}, {"name":"endAudioList","type":"list","value":"int"}]}]]);

---@param k number
---@return GCCfg
function GCCfg:Get(k)
	return GCCfg[k]
end

--battle.xml
--技能相关/GC表现表.txt
