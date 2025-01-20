
---@class Name2Cfg
---@field id number
---@field name string
Name2Cfg = FLuaCfgInterface.RegisTable([[{"name":"Name2Cfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}]}]]);

---@param k number
---@return Name2Cfg
function Name2Cfg:Get(k)
	return Name2Cfg[k]
end

--name.xml
--随机名字/前缀.txt
