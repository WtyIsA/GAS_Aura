
---@class RankCfg
---@field id number
---@field RankType number
---@field ZhanJuType ArrayList<number>
---@field BossType ArrayList<number>
---@field RankText string
---@field ScoreText string
---@field IsPercent boolean
RankCfg = FLuaCfgInterface.RegisTable([[{"name":"RankCfg","fields":[{"name":"id","type":"int"}, {"name":"RankType","type":"int"}, {"name":"ZhanJuType","type":"list","value":"int"}, {"name":"BossType","type":"list","value":"int"}, {"name":"RankText","type":"string"}, {"name":"ScoreText","type":"string"}, {"name":"IsPercent","type":"bool"}]}]]);

---@param k number
---@return RankCfg
function RankCfg:Get(k)
	return RankCfg[k]
end

--rank.xml
--排行榜相关/排行榜表.txt
