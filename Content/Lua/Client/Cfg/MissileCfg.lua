
---@class MissileCfg
---@field id number
---@field blueprint string
---@field hitGE number
---@field hitAudio number
---@field hitEffect number
---@field manMontage number
---@field womanMontage number
---@field acrossAudio ArrayList<number>
---@field bCanReflect boolean
---@field params ArrayList<string>
---@field groundModel string
MissileCfg = FLuaCfgInterface.RegisTable([[{"name":"MissileCfg","fields":[{"name":"id","type":"int"}, {"name":"blueprint","type":"string"}, {"name":"hitGE","type":"int"}, {"name":"hitAudio","type":"int"}, {"name":"hitEffect","type":"int"}, {"name":"manMontage","type":"int"}, {"name":"womanMontage","type":"int"}, {"name":"acrossAudio","type":"list","value":"int"}, {"name":"bCanReflect","type":"bool"}, {"name":"params","type":"list","value":"string"}, {"name":"groundModel","type":"string"}]}]]);

---@param k number
---@return MissileCfg
function MissileCfg:Get(k)
	return MissileCfg[k]
end

--item.xml
--物品相关/投掷物表.txt
