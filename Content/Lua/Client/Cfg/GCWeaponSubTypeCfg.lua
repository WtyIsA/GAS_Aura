require ("WeaponSubTypeAndGCList")

---@class GCWeaponSubTypeCfg
---@field id number
---@field weaponSubTypeAndGC HashMap<number, WeaponSubTypeAndGCList>
GCWeaponSubTypeCfg = FLuaCfgInterface.RegisTable([[{"name":"GCWeaponSubTypeCfg","fields":[{"name":"id","type":"int"}, {"name":"weaponSubTypeAndGC","type":"map","value":"WeaponSubTypeAndGCList","key":"int"}]}]]);

---@param k number
---@return GCWeaponSubTypeCfg
function GCWeaponSubTypeCfg:Get(k)
	return GCWeaponSubTypeCfg[k]
end

--battle.xml
--技能相关/GC施加武器类型表.txt
