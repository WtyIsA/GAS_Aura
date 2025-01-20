
---@class MultiTextureCfg
---@field id number
---@field domestic string
---@field oversea string
MultiTextureCfg = FLuaCfgInterface.RegisTable([[{"name":"MultiTextureCfg","fields":[{"name":"id","type":"int"}, {"name":"domestic","type":"string"}, {"name":"oversea","type":"string"}]}]]);

---@param k number
---@return MultiTextureCfg
function MultiTextureCfg:Get(k)
	return MultiTextureCfg[k]
end

--multilanguage.xml
--字符串/多语言图片表.txt
