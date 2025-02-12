
---@class BulletCfg
---@field id number
---@field blueprint string
---@field hitAudio number
---@field hitEffect number
---@field explodeAudio number
---@field explodeEffect number
---@field acrossAudio ArrayList<number>
---@field bCanReflect boolean
BulletCfg = FLuaCfgInterface.RegisTable([[{"name":"BulletCfg","fields":[{"name":"id","type":"int"}, {"name":"blueprint","type":"string"}, {"name":"hitAudio","type":"int"}, {"name":"hitEffect","type":"int"}, {"name":"explodeAudio","type":"int"}, {"name":"explodeEffect","type":"int"}, {"name":"acrossAudio","type":"list","value":"int"}, {"name":"bCanReflect","type":"bool"}]}]]);

---@param k number
---@return BulletCfg
function BulletCfg:Get(k)
	return BulletCfg[k]
end

--battle.xml
--战斗相关/子弹表.txt
