
---@class ActivityCfg
---@field id number
---@field activityType number
---@field startTime string
---@field endTime string
---@field timeZoneId string
---@field name string
---@field activityLimitType number
ActivityCfg = FLuaCfgInterface.RegisTable([[{"name":"ActivityCfg","fields":[{"name":"id","type":"int"}, {"name":"activityType","type":"int"}, {"name":"startTime","type":"string"}, {"name":"endTime","type":"string"}, {"name":"timeZoneId","type":"string"}, {"name":"name","type":"string"}, {"name":"activityLimitType","type":"int"}]}]]);

---@param k number
---@return ActivityCfg
function ActivityCfg:Get(k)
	return ActivityCfg[k]
end

--activity.xml
--活动相关/活动时间总表.txt
