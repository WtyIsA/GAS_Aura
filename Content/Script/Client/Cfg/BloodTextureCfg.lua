
---@class BloodTextureCfg
---@field id number
---@field path string
BloodTextureCfg = FLuaCfgInterface.RegisTable([[{"name":"BloodTextureCfg","fields":[{"name":"id","type":"int"}, {"name":"path","type":"string"}]}]]);

---@param k number
---@return BloodTextureCfg
function BloodTextureCfg:Get(k)
	return BloodTextureCfg[k]
end

--blood.xml
--流血蒙版表.txt
