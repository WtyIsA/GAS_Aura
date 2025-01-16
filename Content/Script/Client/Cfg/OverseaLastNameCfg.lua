
---@class OverseaLastNameCfg
---@field id number
---@field name string
OverseaLastNameCfg = FLuaCfgInterface.RegisTable([[{"name":"OverseaLastNameCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}]}]]);

---@param k number
---@return OverseaLastNameCfg
function OverseaLastNameCfg:Get(k)
	return OverseaLastNameCfg[k]
end

--name.xml
--随机名字/海外名字库/名字.txt
