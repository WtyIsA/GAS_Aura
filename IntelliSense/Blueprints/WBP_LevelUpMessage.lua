---@class WBP_LevelUpMessage : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public MessageAnimation UWidgetAnimation
---@field public Text_Level UTextBlock
local WBP_LevelUpMessage = {}

---Called after the underlying slate widget is constructed.  Depending on how the slate object is used
---this event may be called multiple times due to adding and removing from the hierarchy.
---If you need a true called-once-when-created event, use OnInitialized.
function WBP_LevelUpMessage:Construct() end

