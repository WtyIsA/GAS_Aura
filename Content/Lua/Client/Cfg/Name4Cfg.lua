
---@class Name4Cfg
---@field id number
---@field name string
Name4Cfg = FLuaCfgInterface.RegisTable([[{"name":"Name4Cfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}]}]]);

---@param k number
---@return Name4Cfg
function Name4Cfg:Get(k)
	return Name4Cfg[k]
end

--name.xml
--随机名字/后后缀.txt
