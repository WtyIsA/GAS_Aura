---@class WBP_EnemyHealthBar : WBP_ProgressBar
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public BPEnemyBase BP_EnemyBase_C
---@field public Health number
---@field public MaxHealth number
local WBP_EnemyHealthBar = {}

---Ticks this widget.  Override in derived classes, but always call the parent implementation.
---@param MyGeometry FGeometry
---@param InDeltaTime number
function WBP_EnemyHealthBar:Tick(MyGeometry, InDeltaTime) end

function WBP_EnemyHealthBar:WidgetControllerSet() end

---@param NewValue number
function WBP_EnemyHealthBar:OnHealthChanged_Event(NewValue) end

---@param NewValue number
function WBP_EnemyHealthBar:OnMaxHealthChanged_Event(NewValue) end

