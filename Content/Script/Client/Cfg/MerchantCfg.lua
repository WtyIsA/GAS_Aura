require ("SellingInfo")
require ("ForgeInfo")
require ("SalesMoneyType")
require ("AcquisitionMoneyType")

---@class MerchantCfg
---@field id number
---@field name string
---@field icon string
---@field desc string
---@field isForge boolean
---@field maxSellingSlotsNum number
---@field sellingStage ArrayList<SellingInfo>
---@field maxForgeSlotsNum number
---@field forgeStage ArrayList<ForgeInfo>
---@field acquisitionType ArrayList<number>
---@field merchantType number
---@field sellingMoney ArrayList<SalesMoneyType>
---@field acquisitionMoney ArrayList<AcquisitionMoneyType>
MerchantCfg = FLuaCfgInterface.RegisTable([[{"name":"MerchantCfg","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"icon","type":"string"}, {"name":"desc","type":"string"}, {"name":"isForge","type":"bool"}, {"name":"maxSellingSlotsNum","type":"int"}, {"name":"sellingStage","type":"list","value":"SellingInfo"}, {"name":"maxForgeSlotsNum","type":"int"}, {"name":"forgeStage","type":"list","value":"ForgeInfo"}, {"name":"acquisitionType","type":"list","value":"int"}, {"name":"merchantType","type":"int"}, {"name":"sellingMoney","type":"list","value":"SalesMoneyType"}, {"name":"acquisitionMoney","type":"list","value":"AcquisitionMoneyType"}]}]]);

---@param k number
---@return MerchantCfg
function MerchantCfg:Get(k)
	return MerchantCfg[k]
end

--merchant.xml
--商人相关/商人配置表.txt
