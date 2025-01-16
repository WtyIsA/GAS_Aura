
---@class PickUpAudioCfg
---@field id number
---@field pickUpAudio number
---@field dropAudio number
---@field uiPickUpAudio number
---@field uiDropAudio number
---@field equipAudio number
PickUpAudioCfg = FLuaCfgInterface.RegisTable([[{"name":"PickUpAudioCfg","fields":[{"name":"id","type":"int"}, {"name":"pickUpAudio","type":"int"}, {"name":"dropAudio","type":"int"}, {"name":"uiPickUpAudio","type":"int"}, {"name":"uiDropAudio","type":"int"}, {"name":"equipAudio","type":"int"}]}]]);

---@param k number
---@return PickUpAudioCfg
function PickUpAudioCfg:Get(k)
	return PickUpAudioCfg[k]
end

--item.xml
--物品相关/物品拾取音效表.txt
