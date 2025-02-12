
---@class OverseaFirstNameCfg
---@field id number
---@field name string
OverseaFirstNameCfg = FLuaCfgInterface.RegisTable([[{"name":"OverseaFirstNameCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}]}]]);

---@param k number
---@return OverseaFirstNameCfg
function OverseaFirstNameCfg:Get(k)
	return OverseaFirstNameCfg[k]
end

--name.xml
--随机名字/海外名字库/姓氏.txt
