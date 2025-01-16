
---@class NamePatternCfg
---@field id number
---@field content string
NamePatternCfg = FLuaCfgInterface.RegisTable([[{"name":"NamePatternCfg","fields":[{"name":"id","type":"int"}, {"name":"content","type":"string"}]}]]);

---@param k number
---@return NamePatternCfg
function NamePatternCfg:Get(k)
	return NamePatternCfg[k]
end

--name.xml
--随机名字/起名限制字符表.txt
