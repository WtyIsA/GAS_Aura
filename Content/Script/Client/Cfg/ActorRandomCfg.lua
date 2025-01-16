require ("ActorRandomBean")

---@class ActorRandomCfg
---@field id number
---@field actorProperties ArrayList<ActorRandomBean>
ActorRandomCfg = FLuaCfgInterface.RegisTable([[{"name":"ActorRandomCfg","fields":[{"name":"id","type":"int"}, {"name":"actorProperties","type":"list","value":"ActorRandomBean"}]}]]);

---@param k number
---@return ActorRandomCfg
function ActorRandomCfg:Get(k)
	return ActorRandomCfg[k]
end

--battle.xml
--刷新表.txt
