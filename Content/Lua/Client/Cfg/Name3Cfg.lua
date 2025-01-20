
---@class Name3Cfg
---@field id number
---@field name string
Name3Cfg = FLuaCfgInterface.RegisTable([[{"name":"Name3Cfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}]}]]);

---@param k number
---@return Name3Cfg
function Name3Cfg:Get(k)
	return Name3Cfg[k]
end

--name.xml
--随机名字/后缀.txt
