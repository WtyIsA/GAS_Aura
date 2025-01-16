
---@class AppearanceCfg
---@field id number
---@field quality number
---@field name string
---@field desc string
---@field heroId number
---@field icon string
---@field headModelId number
---@field bodyModelId number
---@field attribution number
---@field costNum number
---@field aniId number
AppearanceCfg = FLuaCfgInterface.RegisTable([[{"name":"AppearanceCfg","fields":[{"name":"id","type":"int"}, {"name":"quality","type":"int"}, {"name":"name","type":"string"}, {"name":"desc","type":"string"}, {"name":"heroId","type":"int"}, {"name":"icon","type":"string"}, {"name":"headModelId","type":"int"}, {"name":"bodyModelId","type":"int"}, {"name":"attribution","type":"int"}, {"name":"costNum","type":"int"}, {"name":"aniId","type":"int"}]}]]);

---@param k number
---@return AppearanceCfg
function AppearanceCfg:Get(k)
	return AppearanceCfg[k]
end

--appearance.xml
--皮肤相关/英雄皮肤表.txt
