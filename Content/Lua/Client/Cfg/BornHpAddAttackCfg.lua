
---@class BornHpAddAttackCfg
---@field id number
---@field values ArrayList<number>
BornHpAddAttackCfg = FLuaCfgInterface.RegisTable([[{"name":"BornHpAddAttackCfg","fields":[{"name":"id","type":"int"}, {"name":"values","type":"list","value":"int"}]}]]);

---@param k number
---@return BornHpAddAttackCfg
function BornHpAddAttackCfg:Get(k)
	return BornHpAddAttackCfg[k]
end

--battle.xml
--技能相关/野蛮人被动加攻技能效果表.txt
