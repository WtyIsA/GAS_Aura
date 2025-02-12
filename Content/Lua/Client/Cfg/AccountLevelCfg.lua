
---@class AccountLevelCfg
---@field id number
---@field needExp number
AccountLevelCfg = FLuaCfgInterface.RegisTable([[{"name":"AccountLevelCfg","fields":[{"name":"id","type":"int"}, {"name":"needExp","type":"int"}]}]]);

---@param k number
---@return AccountLevelCfg
function AccountLevelCfg:Get(k)
	return AccountLevelCfg[k]
end

--accountLevel.xml
--账号相关/账号升级表.txt
