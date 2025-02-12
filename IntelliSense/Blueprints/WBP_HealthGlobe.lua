---@class WBP_HealthGlobe : WBP_GlobeProgressBar
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public Health number
---@field public MaxHealth number
---@field public BPOverlayWidgetController BP_OverlayWidgetController_C
local WBP_HealthGlobe = {}

---@param NewValue number
function WBP_HealthGlobe:OnHealthChanged_Event(NewValue) end

---@param NewValue number
function WBP_HealthGlobe:OnMaxHealthChanged_Event(NewValue) end

function WBP_HealthGlobe:WidgetControllerSet() end

---Ticks this widget.  Override in derived classes, but always call the parent implementation.
---@param MyGeometry FGeometry
---@param InDeltaTime number
function WBP_HealthGlobe:Tick(MyGeometry, InDeltaTime) end

