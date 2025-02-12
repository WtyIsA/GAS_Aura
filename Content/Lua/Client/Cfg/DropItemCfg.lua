require ("DropDetailInfoSet")
require ("DropDetailInfo")
require ("FixDrop")

---@class DropItemCfg
---@field id number
---@field fixDrops ArrayList<FixDrop>
---@field dropSetList ArrayList<DropDetailInfoSet>
DropItemCfg = FLuaCfgInterface.RegisTable([[{"name":"DropItemCfg","fields":[{"name":"id","type":"int"}, {"name":"fixDrops","type":"list","value":"FixDrop"}, {"name":"dropSetList","type":"list","value":"DropDetailInfoSet"}]}]]);

---@param k number
---@return DropItemCfg
function DropItemCfg:Get(k)
	return DropItemCfg[k]
end

--drop.xml
--物品相关/掉落母表.txt
