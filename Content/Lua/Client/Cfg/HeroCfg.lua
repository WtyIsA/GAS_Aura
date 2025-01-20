
---@class HeroCfg
---@field id number
---@field name string
---@field allowedWeaponType ArrayList<number>
---@field allowedArmorType ArrayList<number>
---@field sort number
---@field heroShape number
---@field bpPath string
---@field heroColor number
---@field initialAppearanceId number
---@field nakedFaceId number
---@field nakedHairId number
---@field halfHelmetHairId number
---@field fullHelmetHairId number
---@field topHelmetHairId number
---@field nakedChestId number
---@field nakedThighId number
---@field nakedShoesId number
---@field nakedGlovesId number
---@field bigCubeIcon string
---@field cubeIcon string
---@field tag string
---@field levelToAttrInit ArrayList<number>
---@field hardRank number
---@field damageRank number
---@field survivalRank number
---@field assistRank number
---@field agilityRank number
---@field attributedGraph string
---@field itemIds ArrayList<number>
---@field isMan boolean
---@field extraActiveSkillId HashMap<number, number>
---@field extraPassiveSkillId HashMap<number, number>
---@field initialActiveSkillIds ArrayList<number>
---@field initialPassiveSkillIds ArrayList<number>
---@field ActiveSkillSlotUnlockLevel ArrayList<number>
---@field PassiveSkillSlotUnlockLevel ArrayList<number>
---@field ownHideSkillIds ArrayList<number>
---@field playerSoundCfgId number
---@field AttackFactorCfgId number
---@field heroTag ArrayList<number>
---@field heroHallAnim number
HeroCfg = FLuaCfgInterface.RegisTable([[{"name":"HeroCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"allowedWeaponType","type":"list","value":"int"}, {"name":"allowedArmorType","type":"list","value":"int"}, {"name":"sort","type":"int"}, {"name":"heroShape","type":"int"}, {"name":"bpPath","type":"string"}, {"name":"heroColor","type":"int"}, {"name":"initialAppearanceId","type":"int"}, {"name":"nakedFaceId","type":"int"}, {"name":"nakedHairId","type":"int"}, {"name":"halfHelmetHairId","type":"int"}, {"name":"fullHelmetHairId","type":"int"}, {"name":"topHelmetHairId","type":"int"}, {"name":"nakedChestId","type":"int"}, {"name":"nakedThighId","type":"int"}, {"name":"nakedShoesId","type":"int"}, {"name":"nakedGlovesId","type":"int"}, {"name":"bigCubeIcon","type":"string"}, {"name":"cubeIcon","type":"string"}, {"name":"tag","type":"string"}, {"name":"levelToAttrInit","type":"list","value":"int"}, {"name":"hardRank","type":"int"}, {"name":"damageRank","type":"int"}, {"name":"survivalRank","type":"int"}, {"name":"assistRank","type":"int"}, {"name":"agilityRank","type":"int"}, {"name":"attributedGraph","type":"string"}, {"name":"itemIds","type":"list","value":"int"}, {"name":"isMan","type":"bool"}, {"name":"extraActiveSkillId","type":"map","value":"int","key":"int"}, {"name":"extraPassiveSkillId","type":"map","value":"int","key":"int"}, {"name":"initialActiveSkillIds","type":"list","value":"int"}, {"name":"initialPassiveSkillIds","type":"list","value":"int"}, {"name":"ActiveSkillSlotUnlockLevel","type":"list","value":"int"}, {"name":"PassiveSkillSlotUnlockLevel","type":"list","value":"int"}, {"name":"ownHideSkillIds","type":"list","value":"int"}, {"name":"playerSoundCfgId","type":"int"}, {"name":"AttackFactorCfgId","type":"int"}, {"name":"heroTag","type":"list","value":"int"}, {"name":"heroHallAnim","type":"int"}]}]]);

---@param k number
---@return HeroCfg
function HeroCfg:Get(k)
	return HeroCfg[k]
end

--hero.xml
--英雄相关/英雄表.txt
