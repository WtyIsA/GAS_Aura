
---@class GmPrivilegesCfg
---@field id number
---@field gmName string
---@field gmLevel ArrayList<number>
GmPrivilegesCfg = FLuaCfgInterface.RegisTable([[{"name":"GmPrivilegesCfg","fields":[{"name":"id","type":"int"}, {"name":"gmName","type":"string"}, {"name":"gmLevel","type":"list","value":"int"}]}]]);

---@param k number
---@return GmPrivilegesCfg
function GmPrivilegesCfg:Get(k)
	return GmPrivilegesCfg[k]
end

--gmprivileges.xml
--GM权限表.txt
