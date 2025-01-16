
---@class WeaponAttackIconCfg
---@field id number
---@field icon string
WeaponAttackIconCfg = FLuaCfgInterface.RegisTable([[{"name":"WeaponAttackIconCfg","fields":[{"name":"id","type":"int"}, {"name":"icon","type":"string"}]}]]);

---@param k number
---@return WeaponAttackIconCfg
function WeaponAttackIconCfg:Get(k)
	return WeaponAttackIconCfg[k]
end

--weapon.xml
--装备相关/武器对应普攻图标表.txt
