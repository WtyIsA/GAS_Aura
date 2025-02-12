---@class WBP_TextValueButtonRow : WBP_TextValueRow
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public WBP_Button WBP_Button_C
---@field public AttributeMenuWidgetController UAttributeMenuWidgetController
local WBP_TextValueButtonRow = {}

---@param Enabled boolean
function WBP_TextValueButtonRow:SetButtonEnabled(Enabled) end

---Called after the underlying slate widget is constructed.  Depending on how the slate object is used
---this event may be called multiple times due to adding and removing from the hierarchy.
---If you need a true called-once-when-created event, use OnInitialized.
function WBP_TextValueButtonRow:Construct() end

---@param Info FAuraAttributeInfo
function WBP_TextValueButtonRow:AttributeInfoDelegate_Event(Info) end

function WBP_TextValueButtonRow:OnClicked_Event() end

