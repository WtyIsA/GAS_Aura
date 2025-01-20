---@class WBP_EquippedSpellRow : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public HidePassiveBox UWidgetAnimation
---@field public PassiveSelectionAnimation UWidgetAnimation
---@field public HideOffensiveBox UWidgetAnimation
---@field public OffensiveSelectionAnimation UWidgetAnimation
---@field public Globe_1 WBP_EquippedRow_Button_C
---@field public Globe_2 WBP_EquippedRow_Button_C
---@field public Globe_3 WBP_EquippedRow_Button_C
---@field public Globe_4 WBP_EquippedRow_Button_C
---@field public Globe_LMB WBP_EquippedRow_Button_C
---@field public Globe_Passive_1 WBP_EquippedRow_Button_C
---@field public Globe_Passive_2 WBP_EquippedRow_Button_C
---@field public Globe_RMB WBP_EquippedRow_Button_C
---@field public Image_OffensiveSelectionBox UImage
---@field public Image_PassiveSelectionBox UImage
---@field public SizeBox_Root USizeBox
---@field public BoxWidth number
---@field public BoxHeight number
local WBP_EquippedSpellRow = {}

function WBP_EquippedSpellRow:SetWidgetControllers() end

function WBP_EquippedSpellRow:UpdateBoxSize() end

---Called by both the game and the editor.  Allows users to run initial setup for their widgets to better preview
---the setup in the designer and since generally that same setup code is required at runtime, it's called there
---as well.
---**WARNING**
---This is intended purely for cosmetic updates using locally owned data, you can not safely access any game related
---state, if you call something that doesn't expect to be run at editor time, you may crash the editor.
---In the event you save the asset with blueprint code that causes a crash on evaluation.  You can turn off
---PreConstruct evaluation in the Widget Designer settings in the Editor Preferences.
---@param IsDesignTime boolean
function WBP_EquippedSpellRow:PreConstruct(IsDesignTime) end

function WBP_EquippedSpellRow:WidgetControllerSet() end

