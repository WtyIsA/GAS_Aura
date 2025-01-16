
---@class AudioCfg
---@field id number
---@field bankName string
---@field audioParams string
---@field bCanTriggerAgain boolean
---@field audioDuration number
---@field FadeInTime number
---@field FadeOutTime number
AudioCfg = FLuaCfgInterface.RegisTable([[{"name":"AudioCfg","fields":[{"name":"id","type":"int"}, {"name":"bankName","type":"string"}, {"name":"audioParams","type":"string"}, {"name":"bCanTriggerAgain","type":"bool"}, {"name":"audioDuration","type":"float"}, {"name":"FadeInTime","type":"float"}, {"name":"FadeOutTime","type":"float"}]}]]);

---@param k number
---@return AudioCfg
function AudioCfg:Get(k)
	return AudioCfg[k]
end

--audio.xml
--音效表.txt
