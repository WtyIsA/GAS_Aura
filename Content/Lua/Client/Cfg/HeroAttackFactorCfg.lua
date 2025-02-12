require ("WeaponDamageValue")

---@class HeroAttackFactorCfg
---@field id number
---@field weaponDamageList ArrayList<WeaponDamageValue>
HeroAttackFactorCfg = FLuaCfgInterface.RegisTable([[{"name":"HeroAttackFactorCfg","fields":[{"name":"id","type":"int"}, {"name":"weaponDamageList","type":"list","value":"WeaponDamageValue"}]}]]);

---@param k number
---@return HeroAttackFactorCfg
function HeroAttackFactorCfg:Get(k)
	return HeroAttackFactorCfg[k]
end

--hero.xml
--英雄相关/英雄使用武器的伤害倍率.txt
