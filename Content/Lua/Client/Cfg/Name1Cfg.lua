
---@class Name1Cfg
---@field id number
---@field name string
Name1Cfg = FLuaCfgInterface.RegisTable([[{"name":"Name1Cfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}]}]]);

---@param k number
---@return Name1Cfg
function Name1Cfg:Get(k)
	return Name1Cfg[k]
end

--name.xml
--随机名字/前前缀.txt
