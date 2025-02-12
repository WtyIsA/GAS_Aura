---@class WBP_XPBar : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public ProgressBar_XP UProgressBar
---@field public BPOverlayWidgetController BP_OverlayWidgetController_C
local WBP_XPBar = {}

function WBP_XPBar:WidgetControllerSet() end

---@param NewValue number
function WBP_XPBar:OnXPPercentChangedDelegate_Event(NewValue) end

