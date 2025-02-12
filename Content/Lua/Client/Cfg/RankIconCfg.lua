
---@class RankIconCfg
---@field id number
---@field icon string
RankIconCfg = FLuaCfgInterface.RegisTable([[{"name":"RankIconCfg","fields":[{"name":"id","type":"int"}, {"name":"icon","type":"string"}]}]]);

---@param k number
---@return RankIconCfg
function RankIconCfg:Get(k)
	return RankIconCfg[k]
end

--rank.xml
--排行榜相关/排名对应图标表.txt
