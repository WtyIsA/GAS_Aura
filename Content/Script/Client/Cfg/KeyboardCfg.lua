
---@class KeyboardCfg
---@field id number
---@field key string
---@field iconPath string
---@field keyShowName string
KeyboardCfg = FLuaCfgInterface.RegisTable([[{"name":"KeyboardCfg","fields":[{"name":"id","type":"int"}, {"name":"key","type":"string"}, {"name":"iconPath","type":"string"}, {"name":"keyShowName","type":"string"}]}]]);

---@param k number
---@return KeyboardCfg
function KeyboardCfg:Get(k)
	return KeyboardCfg[k]
end

--set.xml
--设置相关/按键表.txt
