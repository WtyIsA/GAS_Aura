require ("SalesType")

---@class CostType
---@field selling SalesType
---@field acquisition SalesType
CostType = nil

FLuaCfgInterface.RegisBean([[{"name":"CostType","fields":[{"name":"selling","type":"SalesType"}, {"name":"acquisition","type":"SalesType"}]}]]);

--merchant.xml

