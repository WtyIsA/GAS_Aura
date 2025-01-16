
---@class SystemMailCfg
---@field id number
---@field gsLevelMin number
---@field gsLevelMax number
---@field bsLevelMin number
---@field bsLevelMax number
---@field period number
SystemMailCfg = FLuaCfgInterface.RegisTable([[{"name":"SystemMailCfg","fields":[{"name":"id","type":"int"}, {"name":"gsLevelMin","type":"int"}, {"name":"gsLevelMax","type":"int"}, {"name":"bsLevelMin","type":"int"}, {"name":"bsLevelMax","type":"int"}, {"name":"period","type":"int"}]}]]);

---@param k number
---@return SystemMailCfg
function SystemMailCfg:Get(k)
	return SystemMailCfg[k]
end

--mail.xml
--邮件/邮件.txt
