require ("WeaponActionValue")

---@class ShowActionCfg
---@field id number
---@field leftActionId number
---@field middleActionId number
---@field rightActionId number
---@field actionList ArrayList<WeaponActionValue>
ShowActionCfg = FLuaCfgInterface.RegisTable([[{"name":"ShowActionCfg","fields":[{"name":"id","type":"int"}, {"name":"leftActionId","type":"int"}, {"name":"middleActionId","type":"int"}, {"name":"rightActionId","type":"int"}, {"name":"actionList","type":"list","value":"WeaponActionValue"}]}]]);

---@param k number
---@return ShowActionCfg
function ShowActionCfg:Get(k)
	return ShowActionCfg[k]
end

--hero.xml
--英雄相关/展示动作表.txt
