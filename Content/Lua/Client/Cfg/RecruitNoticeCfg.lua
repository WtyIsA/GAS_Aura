
---@class RecruitNoticeCfg
---@field id number
---@field notice string
RecruitNoticeCfg = FLuaCfgInterface.RegisTable([[{"name":"RecruitNoticeCfg","fields":[{"name":"id","type":"int"}, {"name":"notice","type":"string"}]}]]);

---@param k number
---@return RecruitNoticeCfg
function RecruitNoticeCfg:Get(k)
	return RecruitNoticeCfg[k]
end

--troop.xml
--招募相关/招募宣言表.txt
