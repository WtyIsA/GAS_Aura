
---@class Strings
---@field id number
---@field content string
Strings = FLuaCfgInterface.RegisTable([[{"name":"Strings","fields":[{"name":"id","type":"int"}, {"name":"content","type":"string"}]}]]);

---@param k number
---@return Strings
function Strings:Get(k)
	return Strings[k]
end

--import.xml
--字符串/字符串表.txt
