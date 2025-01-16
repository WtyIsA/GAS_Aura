
---@class LoadingTextureCfg
---@field id number
---@field path string
LoadingTextureCfg = FLuaCfgInterface.RegisTable([[{"name":"LoadingTextureCfg","fields":[{"name":"id","type":"int"}, {"name":"path","type":"string"}]}]]);

---@param k number
---@return LoadingTextureCfg
function LoadingTextureCfg:Get(k)
	return LoadingTextureCfg[k]
end

--loading.xml
--Loading相关/Loading图表.txt
