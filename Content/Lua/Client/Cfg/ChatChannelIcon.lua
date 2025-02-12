
---@class ChatChannelIcon
---@field id number
---@field name string
---@field path string
---@field colorId number
ChatChannelIcon = FLuaCfgInterface.RegisTable([[{"name":"ChatChannelIcon","fields":[{"name":"id","type":"int"}, {"name":"name","type":"string"}, {"name":"path","type":"string"}, {"name":"colorId","type":"int"}]}]]);

---@param k number
---@return ChatChannelIcon
function ChatChannelIcon:Get(k)
	return ChatChannelIcon[k]
end

--chat.xml
--聊天相关/聊天频道对应图标表.txt
