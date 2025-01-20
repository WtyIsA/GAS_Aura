
---@class GEWeaponSubTypeCfg
---@field id number
---@field GEList ArrayList<number>
---@field WeaponSubTypeList ArrayList<number>
GEWeaponSubTypeCfg = FLuaCfgInterface.RegisTable([[{"name":"GEWeaponSubTypeCfg","fields":[{"name":"id","type":"int"}, {"name":"GEList","type":"list","value":"int"}, {"name":"WeaponSubTypeList","type":"list","value":"int"}]}]]);

---@param k number
---@return GEWeaponSubTypeCfg
function GEWeaponSubTypeCfg:Get(k)
	return GEWeaponSubTypeCfg[k]
end

--battle.xml
--技能相关/GE施加武器类型表.txt
