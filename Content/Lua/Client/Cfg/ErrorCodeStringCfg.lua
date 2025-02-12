
---@class ErrorCodeStringCfg
---@field code string
---@field errorTip string
ErrorCodeStringCfg = FLuaCfgInterface.RegisTable([[{"name":"ErrorCodeStringCfg","fields":[{"name":"code","type":"string"}, {"name":"errorTip","type":"string"}]}]]);

---@param k string
---@return ErrorCodeStringCfg
function ErrorCodeStringCfg:Get(k)
	return ErrorCodeStringCfg[k]
end

--import.xml
--字符串/错误码对应字符串表.txt
