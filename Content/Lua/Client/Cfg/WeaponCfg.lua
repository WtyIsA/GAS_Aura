require ("ExtraAttributionIdProbability")
require ("ExtraAttributionNumProbability")

---@class WeaponCfg
---@field id number
---@field path string
---@field type number
---@field subType number
---@field slot ArrayList<number>
---@field crossHair number
---@field weaponPhysicalDamage number
---@field weaponMagicalDamage number
---@field weaponAttackTime number
---@field maxBulletNumber number
---@field baseAttribution number
---@field primaryGA number
---@field secondaryGA number
---@field extraGA ArrayList<number>
---@field weaponAudioId number
---@field hitAudioId number
---@field hitEffectId number
---@field weaponImpact number
---@field markPosDamage ArrayList<number>
---@field primaryAttackAnimDamage ArrayList<number>
---@field secondaryAttackAnimDamage ArrayList<number>
---@field weaponSocketOnBody string
---@field weaponSocketOnHand string
---@field weaponBeTakenOutAnim number
---@field manTakeOutWeaponAnim number
---@field manTakeBackWeaponAnim number
---@field manTakeOutWeaponFPPAnim number
---@field manTakeBackWeaponFPPAnim number
---@field womanTakeOutWeaponAnim number
---@field womanTakeBackWeaponAnim number
---@field womanTakeOutWeaponFPPAnim number
---@field womanTakeBackWeaponFPPAnim number
---@field manHallWeaponAnim number
---@field manHallSocketName string
---@field womanHallWeaponAnim number
---@field womanHallSocketName string
---@field extraAttributionNumProbability ArrayList<ExtraAttributionNumProbability>
---@field extraAttributionIdProbability ArrayList<ExtraAttributionIdProbability>
WeaponCfg = FLuaCfgInterface.RegisTable([[{"name":"WeaponCfg","fields":[{"name":"id","type":"int"}, {"name":"path","type":"string"}, {"name":"type","type":"int"}, {"name":"subType","type":"int"}, {"name":"slot","type":"list","value":"int"}, {"name":"crossHair","type":"int"}, {"name":"weaponPhysicalDamage","type":"float"}, {"name":"weaponMagicalDamage","type":"float"}, {"name":"weaponAttackTime","type":"float"}, {"name":"maxBulletNumber","type":"int"}, {"name":"baseAttribution","type":"int"}, {"name":"primaryGA","type":"int"}, {"name":"secondaryGA","type":"int"}, {"name":"extraGA","type":"list","value":"int"}, {"name":"weaponAudioId","type":"int"}, {"name":"hitAudioId","type":"int"}, {"name":"hitEffectId","type":"int"}, {"name":"weaponImpact","type":"int"}, {"name":"markPosDamage","type":"list","value":"float"}, {"name":"primaryAttackAnimDamage","type":"list","value":"float"}, {"name":"secondaryAttackAnimDamage","type":"list","value":"float"}, {"name":"weaponSocketOnBody","type":"string"}, {"name":"weaponSocketOnHand","type":"string"}, {"name":"weaponBeTakenOutAnim","type":"int"}, {"name":"manTakeOutWeaponAnim","type":"int"}, {"name":"manTakeBackWeaponAnim","type":"int"}, {"name":"manTakeOutWeaponFPPAnim","type":"int"}, {"name":"manTakeBackWeaponFPPAnim","type":"int"}, {"name":"womanTakeOutWeaponAnim","type":"int"}, {"name":"womanTakeBackWeaponAnim","type":"int"}, {"name":"womanTakeOutWeaponFPPAnim","type":"int"}, {"name":"womanTakeBackWeaponFPPAnim","type":"int"}, {"name":"manHallWeaponAnim","type":"int"}, {"name":"manHallSocketName","type":"string"}, {"name":"womanHallWeaponAnim","type":"int"}, {"name":"womanHallSocketName","type":"string"}, {"name":"extraAttributionNumProbability","type":"list","value":"ExtraAttributionNumProbability"}, {"name":"extraAttributionIdProbability","type":"list","value":"ExtraAttributionIdProbability"}]}]]);

---@param k number
---@return WeaponCfg
function WeaponCfg:Get(k)
	return WeaponCfg[k]
end

--weapon.xml
--装备相关/武器表.txt
