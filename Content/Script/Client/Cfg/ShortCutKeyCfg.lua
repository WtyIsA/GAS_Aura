
---@class ShortCutKeyCfg
---@field id number
---@field type number
---@field defaultMainKey ArrayList<number>
---@field mainKeyIMCName string
---@field defaultAuxiliaryKey ArrayList<number>
---@field auxiliaryKeyIMCName string
---@field keyName string
---@field keyDescribe string
ShortCutKeyCfg = FLuaCfgInterface.RegisTable([[{"name":"ShortCutKeyCfg","fields":[{"name":"id","type":"int"}, {"name":"type","type":"int"}, {"name":"defaultMainKey","type":"list","value":"int"}, {"name":"mainKeyIMCName","type":"string"}, {"name":"defaultAuxiliaryKey","type":"list","value":"int"}, {"name":"auxiliaryKeyIMCName","type":"string"}, {"name":"keyName","type":"string"}, {"name":"keyDescribe","type":"string"}]}]]);

---@param k number
---@return ShortCutKeyCfg
function ShortCutKeyCfg:Get(k)
	return ShortCutKeyCfg[k]
end

--set.xml
--设置相关/键盘对应表.txt
