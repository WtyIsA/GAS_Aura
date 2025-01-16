require ("DropInfo")

---@class DropItemDetailCfg
---@field id number
---@field subItems ArrayList<DropInfo>
DropItemDetailCfg = FLuaCfgInterface.RegisTable([[{"name":"DropItemDetailCfg","fields":[{"name":"id","type":"int"}, {"name":"subItems","type":"list","value":"DropInfo"}]}]]);

---@param k number
---@return DropItemDetailCfg
function DropItemDetailCfg:Get(k)
	return DropItemDetailCfg[k]
end

--drop.xml
--物品相关/掉落子表.txt
