
---@class LoadingTipsCfg
---@field id number
---@field tips string
LoadingTipsCfg = FLuaCfgInterface.RegisTable([[{"name":"LoadingTipsCfg","fields":[{"name":"id","type":"int"}, {"name":"tips","type":"string"}]}]]);

---@param k number
---@return LoadingTipsCfg
function LoadingTipsCfg:Get(k)
	return LoadingTipsCfg[k]
end

--loading.xml
--Loading相关/LoadingTips表.txt
