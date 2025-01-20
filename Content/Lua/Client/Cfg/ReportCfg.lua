
---@class ReportCfg
---@field type number
---@field reportIds ArrayList<number>
ReportCfg = FLuaCfgInterface.RegisTable([[{"name":"ReportCfg","fields":[{"name":"type","type":"int"}, {"name":"reportIds","type":"list","value":"int"}]}]]);

---@param k number
---@return ReportCfg
function ReportCfg:Get(k)
	return ReportCfg[k]
end

--report.xml
--举报相关/举报配置表.txt
