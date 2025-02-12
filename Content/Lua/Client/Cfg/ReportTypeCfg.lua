
---@class ReportTypeCfg
---@field type number
---@field isLv1Id boolean
---@field lv2Ids ArrayList<number>
---@field name string
---@field desc string
---@field isReportChatContent boolean
ReportTypeCfg = FLuaCfgInterface.RegisTable([[{"name":"ReportTypeCfg","fields":[{"name":"type","type":"int"}, {"name":"isLv1Id","type":"bool"}, {"name":"lv2Ids","type":"list","value":"int"}, {"name":"name","type":"string"}, {"name":"desc","type":"string"}, {"name":"isReportChatContent","type":"bool"}]}]]);

---@param k number
---@return ReportTypeCfg
function ReportTypeCfg:Get(k)
	return ReportTypeCfg[k]
end

--report.xml
--举报相关/举报内容表.txt
