
---@class StringsAudioCfg
---@field id number
---@field audioId number
StringsAudioCfg = FLuaCfgInterface.RegisTable([[{"name":"StringsAudioCfg","fields":[{"name":"id","type":"int"}, {"name":"audioId","type":"int"}]}]]);

---@param k number
---@return StringsAudioCfg
function StringsAudioCfg:Get(k)
	return StringsAudioCfg[k]
end

--import.xml
--字符串/字符串音效表.txt
