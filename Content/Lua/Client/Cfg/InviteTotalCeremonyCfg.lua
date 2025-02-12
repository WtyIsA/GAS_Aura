
---@class InviteTotalCeremonyCfg
---@field id number
---@field MoneyType number
---@field itemId number
---@field itemNum number
InviteTotalCeremonyCfg = FLuaCfgInterface.RegisTable([[{"name":"InviteTotalCeremonyCfg","fields":[{"name":"id","type":"int"}, {"name":"MoneyType","type":"int"}, {"name":"itemId","type":"int"}, {"name":"itemNum","type":"int"}]}]]);

---@param k number
---@return InviteTotalCeremonyCfg
function InviteTotalCeremonyCfg:Get(k)
	return InviteTotalCeremonyCfg[k]
end

--activity.xml
--活动相关/邀请有礼累计奖励.txt
