---@class WBP_OffensiveSpellTree : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public SizeBox_Root USizeBox
---@field public WBP_SpellGlobe_Button WBP_SpellGlobe_Button_C
---@field public WBP_SpellGlobe_Button_1 WBP_SpellGlobe_Button_C
---@field public WBP_SpellGlobe_Button_2 WBP_SpellGlobe_Button_C
---@field public WBP_SpellGlobe_Button_3 WBP_SpellGlobe_Button_C
---@field public WBP_SpellGlobe_Button_4 WBP_SpellGlobe_Button_C
---@field public WBP_SpellGlobe_Button_5 WBP_SpellGlobe_Button_C
---@field public WBP_SpellGlobe_Button_6 WBP_SpellGlobe_Button_C
---@field public WBP_SpellGlobe_Button_7 WBP_SpellGlobe_Button_C
---@field public WBP_SpellGlobe_Button_8 WBP_SpellGlobe_Button_C
---@field public BoxWidth number
---@field public BoxHeight number
---@field public OnOffensiveSpellGlobeSelected MulticastDelegate
local WBP_OffensiveSpellTree = {}

function WBP_OffensiveSpellTree:DeselectAll() end

function WBP_OffensiveSpellTree:SetWidgetControllers() end

function WBP_OffensiveSpellTree:UpdateBoxSize() end

---Called by both the game and the editor.  Allows users to run initial setup for their widgets to better preview
---the setup in the designer and since generally that same setup code is required at runtime, it's called there
---as well.
---**WARNING**
---This is intended purely for cosmetic updates using locally owned data, you can not safely access any game related
---state, if you call something that doesn't expect to be run at editor time, you may crash the editor.
---In the event you save the asset with blueprint code that causes a crash on evaluation.  You can turn off
---PreConstruct evaluation in the Widget Designer settings in the Editor Preferences.
---@param IsDesignTime boolean
function WBP_OffensiveSpellTree:PreConstruct(IsDesignTime) end

---Called after the underlying slate widget is constructed.  Depending on how the slate object is used
---this event may be called multiple times due to adding and removing from the hierarchy.
---If you need a true called-once-when-created event, use OnInitialized.
function WBP_OffensiveSpellTree:Construct() end

function WBP_OffensiveSpellTree:WidgetControllerSet() end

---@param SelectedGlobe WBP_SpellGlobe_Button_C
function WBP_OffensiveSpellTree:OnSpellGlobeSelected_Event(SelectedGlobe) end

function WBP_OffensiveSpellTree:OnOffensiveSpellGlobeSelected__DelegateSignature() end

