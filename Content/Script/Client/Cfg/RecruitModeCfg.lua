
---@class RecruitModeCfg
---@field id number
---@field name string
---@field maps ArrayList<number>
RecruitModeCfg = FLuaCfgInterface.RegisTable([[{"name":"RecruitModeCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"maps","type":"list","value":"int"}]}]]);

---@param k number
---@return RecruitModeCfg
function RecruitModeCfg:Get(k)
	return RecruitModeCfg[k]
end

--troop.xml
--招募相关/招募模式表.txt
