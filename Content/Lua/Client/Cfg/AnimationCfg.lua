
---@class AnimationCfg
---@field id number
---@field animPath string
AnimationCfg = FLuaCfgInterface.RegisTable([[{"name":"AnimationCfg","fields":[{"name":"id","type":"int"}, {"name":"animPath","type":"string"}]}]]);

---@param k number
---@return AnimationCfg
function AnimationCfg:Get(k)
	return AnimationCfg[k]
end

--animation.xml
--动画表.txt
