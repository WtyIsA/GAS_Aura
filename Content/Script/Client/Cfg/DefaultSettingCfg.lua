
---@class DefaultSettingCfg
---@field id number
---@field value number
---@field options ArrayList<number>
---@field name string
---@field tip string
---@field isSlider boolean
---@field sliderMinNum number
---@field sliderMaxNum number
DefaultSettingCfg = FLuaCfgInterface.RegisTable([[{"name":"DefaultSettingCfg","fields":[{"name":"id","type":"int"}, {"name":"value","type":"int"}, {"name":"options","type":"list","value":"int"}, {"name":"name","type":"string"}, {"name":"tip","type":"string"}, {"name":"isSlider","type":"bool"}, {"name":"sliderMinNum","type":"int"}, {"name":"sliderMaxNum","type":"int"}]}]]);

---@param k number
---@return DefaultSettingCfg
function DefaultSettingCfg:Get(k)
	return DefaultSettingCfg[k]
end

--set.xml
--设置相关/设置表.txt
