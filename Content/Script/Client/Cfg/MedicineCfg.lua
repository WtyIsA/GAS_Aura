
---@class MedicineCfg
---@field id number
---@field blueprint string
---@field GE ArrayList<number>
---@field GC ArrayList<number>
---@field GC_OnHand ArrayList<number>
---@field icon string
---@field bodySocket string
---@field manMontage number
---@field manEndMontage number
---@field womanMontage number
---@field womanEndMontage number
---@field useTime number
MedicineCfg = FLuaCfgInterface.RegisTable([[{"name":"MedicineCfg","fields":[{"name":"id","type":"int"}, {"name":"blueprint","type":"string"}, {"name":"GE","type":"list","value":"int"}, {"name":"GC","type":"list","value":"int"}, {"name":"GC_OnHand","type":"list","value":"int"}, {"name":"icon","type":"string"}, {"name":"bodySocket","type":"string"}, {"name":"manMontage","type":"int"}, {"name":"manEndMontage","type":"int"}, {"name":"womanMontage","type":"int"}, {"name":"womanEndMontage","type":"int"}, {"name":"useTime","type":"float"}]}]]);

---@param k number
---@return MedicineCfg
function MedicineCfg:Get(k)
	return MedicineCfg[k]
end

--item.xml
--物品相关/药品表.txt
