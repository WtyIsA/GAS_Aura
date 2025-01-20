---@class WBP_HealthManaSpells : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public SpellGlobe_1 WBP_SpellGlobe_C
---@field public SpellGlobe_2 WBP_SpellGlobe_C
---@field public SpellGlobe_3 WBP_SpellGlobe_C
---@field public SpellGlobe_4 WBP_SpellGlobe_C
---@field public SpellGlobe_LMB WBP_SpellGlobe_C
---@field public SpellGlobe_Passive_1 WBP_SpellGlobe_C
---@field public SpellGlobe_Passive_2 WBP_SpellGlobe_C
---@field public SpellGlobe_RMB WBP_SpellGlobe_C
---@field public WBP_HealthGlobe WBP_HealthGlobe_C
---@field public WBP_ManaGlobe WBP_ManaGlobe_C
local WBP_HealthManaSpells = {}

function WBP_HealthManaSpells:SetGlobeWidgetControllers() end

function WBP_HealthManaSpells:SetSpellGlobeInputTags() end

---Called by both the game and the editor.  Allows users to run initial setup for their widgets to better preview
---the setup in the designer and since generally that same setup code is required at runtime, it's called there
---as well.
---**WARNING**
---This is intended purely for cosmetic updates using locally owned data, you can not safely access any game related
---state, if you call something that doesn't expect to be run at editor time, you may crash the editor.
---In the event you save the asset with blueprint code that causes a crash on evaluation.  You can turn off
---PreConstruct evaluation in the Widget Designer settings in the Editor Preferences.
---@param IsDesignTime boolean
function WBP_HealthManaSpells:PreConstruct(IsDesignTime) end

function WBP_HealthManaSpells:WidgetControllerSet() end

