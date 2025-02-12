
---@class WeaponAudioCfg
---@field id number
---@field takeAudio number
---@field backAudio number
---@field wieldAudio number
WeaponAudioCfg = FLuaCfgInterface.RegisTable([[{"name":"WeaponAudioCfg","fields":[{"name":"id","type":"int"}, {"name":"takeAudio","type":"int"}, {"name":"backAudio","type":"int"}, {"name":"wieldAudio","type":"int"}]}]]);

---@param k number
---@return WeaponAudioCfg
function WeaponAudioCfg:Get(k)
	return WeaponAudioCfg[k]
end

--weapon.xml
--装备相关/武器音效表.txt
