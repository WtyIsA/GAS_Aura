
---@class KillTypeCfg
---@field id number
---@field name string
KillTypeCfg = FLuaCfgInterface.RegisTable([[{"name":"KillTypeCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}]}]]);

---@param k number
---@return KillTypeCfg
function KillTypeCfg:Get(k)
	return KillTypeCfg[k]
end

--battle.xml
--击杀相关/击杀类型表.txt
