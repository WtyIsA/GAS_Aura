
---@class JarCfg
---@field id number
---@field dropDelayTime number
JarCfg = FLuaCfgInterface.RegisTable([[{"name":"JarCfg","fields":[{"name":"id","type":"int"}, {"name":"dropDelayTime","type":"float"}]}]]);

---@param k number
---@return JarCfg
function JarCfg:Get(k)
	return JarCfg[k]
end

--box.xml
--场景物相关/场景可破坏物表.txt
