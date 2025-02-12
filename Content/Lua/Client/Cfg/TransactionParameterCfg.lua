
---@class TransactionParameterCfg
---@field id number
---@field percentage number
---@field transactionFeesPercentage number
---@field listingFeesPercentage number
---@field transactionBuyListMaxNum number
---@field transactionSellListMaxNum number
---@field transactionAuctionListMaxNum number
---@field equipmentSellMaxNum number
---@field itemSellMaxNum number
---@field equipmentBuyMaxNum number
---@field itemBuyMaxNum number
---@field transactionHangoutTime number
---@field auctionPriceChangeMinPercentage number
---@field auctionCountdownQuality ArrayList<number>
---@field auctionCountdownTime ArrayList<number>
---@field myAuctionRecordNum number
---@field myAuctionRecordTime number
---@field auctionUnlockLevel number
---@field canSellItemTypeList ArrayList<number>
TransactionParameterCfg = FLuaCfgInterface.RegisTable([[{"name":"TransactionParameterCfg","fields":[{"name":"id","type":"int"}, {"name":"percentage","type":"int"}, {"name":"transactionFeesPercentage","type":"int"}, {"name":"listingFeesPercentage","type":"int"}, {"name":"transactionBuyListMaxNum","type":"int"}, {"name":"transactionSellListMaxNum","type":"int"}, {"name":"transactionAuctionListMaxNum","type":"int"}, {"name":"equipmentSellMaxNum","type":"int"}, {"name":"itemSellMaxNum","type":"int"}, {"name":"equipmentBuyMaxNum","type":"int"}, {"name":"itemBuyMaxNum","type":"int"}, {"name":"transactionHangoutTime","type":"int"}, {"name":"auctionPriceChangeMinPercentage","type":"int"}, {"name":"auctionCountdownQuality","type":"list","value":"int"}, {"name":"auctionCountdownTime","type":"list","value":"int"}, {"name":"myAuctionRecordNum","type":"int"}, {"name":"myAuctionRecordTime","type":"int"}, {"name":"auctionUnlockLevel","type":"int"}, {"name":"canSellItemTypeList","type":"list","value":"int"}]}]]);

---@param k number
---@return TransactionParameterCfg
function TransactionParameterCfg:Get(k)
	return TransactionParameterCfg[k]
end

--transaction.xml
--交易行相关/交易行参数表.txt
