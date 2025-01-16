
---@class InviteFriendCeremonyCfg
---@field id number
---@field MoneyType number
---@field itemId number
---@field itemNum number
InviteFriendCeremonyCfg = FLuaCfgInterface.RegisTable([[{"name":"InviteFriendCeremonyCfg","fields":[{"name":"id","type":"int"}, {"name":"MoneyType","type":"int"}, {"name":"itemId","type":"int"}, {"name":"itemNum","type":"int"}]}]]);

---@param k number
---@return InviteFriendCeremonyCfg
function InviteFriendCeremonyCfg:Get(k)
	return InviteFriendCeremonyCfg[k]
end

--activity.xml
--活动相关/邀请有礼好友奖励.txt
