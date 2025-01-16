
---@class MultiLanguageIndexCfg
---@field id number
---@field language string
---@field shorthand string
---@field sdkShorthand string
---@field loginSdkShorthand string
---@field sdkActivity string
---@field protocolUrl string
---@field privateUrl string
---@field chatlanguageId string
---@field channelId number
MultiLanguageIndexCfg = FLuaCfgInterface.RegisTable([[{"name":"MultiLanguageIndexCfg","fields":[{"name":"id","type":"int"}, {"name":"language","type":"string"}, {"name":"shorthand","type":"string"}, {"name":"sdkShorthand","type":"string"}, {"name":"loginSdkShorthand","type":"string"}, {"name":"sdkActivity","type":"string"}, {"name":"protocolUrl","type":"string"}, {"name":"privateUrl","type":"string"}, {"name":"chatlanguageId","type":"string"}, {"name":"channelId","type":"int"}]}]]);

---@param k number
---@return MultiLanguageIndexCfg
function MultiLanguageIndexCfg:Get(k)
	return MultiLanguageIndexCfg[k]
end

--multilanguage.xml
--字符串/语言对应表.txt
