
---@class ModelCfg
---@field id number
---@field modelType number
---@field modelMaterialType number
---@field modelPath string
---@field BpPath string
ModelCfg = FLuaCfgInterface.RegisTable([[{"name":"ModelCfg","fields":[{"name":"id","type":"int"}, {"name":"modelType","type":"int"}, {"name":"modelMaterialType","type":"int"}, {"name":"modelPath","type":"string"}, {"name":"BpPath","type":"string"}]}]]);

---@param k number
---@return ModelCfg
function ModelCfg:Get(k)
	return ModelCfg[k]
end

--appearance.xml
--皮肤相关/模型表.txt
