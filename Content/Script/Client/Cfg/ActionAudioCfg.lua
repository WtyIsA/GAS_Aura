
---@class ActionAudioCfg
---@field id number
---@field walkAudio number
---@field runAudio number
---@field crouchWalkAudio number
---@field jumpAudio number
---@field fallAudio number
---@field toStandAudio number
---@field toCrouchAudio number
ActionAudioCfg = FLuaCfgInterface.RegisTable([[{"name":"ActionAudioCfg","fields":[{"name":"id","type":"int"}, {"name":"walkAudio","type":"int"}, {"name":"runAudio","type":"int"}, {"name":"crouchWalkAudio","type":"int"}, {"name":"jumpAudio","type":"int"}, {"name":"fallAudio","type":"int"}, {"name":"toStandAudio","type":"int"}, {"name":"toCrouchAudio","type":"int"}]}]]);

---@param k number
---@return ActionAudioCfg
function ActionAudioCfg:Get(k)
	return ActionAudioCfg[k]
end

--hero.xml
--英雄相关/行为音效表.txt
