---@class WBP_ManaGlobe : WBP_GlobeProgressBar
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public BPOverlayWidgetController BP_OverlayWidgetController_C
---@field public Mana number
---@field public MaxMana number
local WBP_ManaGlobe = {}

---Ticks this widget.  Override in derived classes, but always call the parent implementation.
---@param MyGeometry FGeometry
---@param InDeltaTime number
function WBP_ManaGlobe:Tick(MyGeometry, InDeltaTime) end

function WBP_ManaGlobe:WidgetControllerSet() end

---@param NewValue number
function WBP_ManaGlobe:OnManaChanged_Event(NewValue) end

---@param NewValue number
function WBP_ManaGlobe:OnMaxManaChanged_Event(NewValue) end

