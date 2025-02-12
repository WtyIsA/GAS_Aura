---@class WBP_MyOverlay : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public AttributeMenuButton WBP_WideButton_C
---@field public SpellMenuButton WBP_WideButton_C
---@field public ValueGlobe_Level WBP_ValueGlobe_C
---@field public WBP_HealthManaSpells WBP_HealthManaSpells_C
---@field public WBP_XPBar WBP_XPBar_C
---@field public BPOverlayWidgetController BP_OverlayWidgetController_C
---@field public AttributeMenuOpen boolean
---@field public MenuPadding number
---@field public SpellMenuOpen boolean
---@field public LevelUpWidget WBP_LevelUpMessage_C
local WBP_MyOverlay = {}

function WBP_MyOverlay:SpellMenuClosed() end

---@return WBP_SpellMenu_C
function WBP_MyOverlay:CreateSpellMenu() end

---@param Value integer
function WBP_MyOverlay:HandleLevelUpMessage(Value) end

function WBP_MyOverlay:InputModeUIOnly() end

function WBP_MyOverlay:AttributeMenuClosed() end

---@return WBP_AttributeMenu_C
function WBP_MyOverlay:CreateAttributeMenu() end

---@param UIWidgetRow FUIWidgetRow
WBP_MyOverlay["Handle Message Widget"] = function(self, UIWidgetRow) end

---Called after the underlying slate widget is constructed.  Depending on how the slate object is used
---this event may be called multiple times due to adding and removing from the hierarchy.
---If you need a true called-once-when-created event, use OnInitialized.
function WBP_MyOverlay:Construct() end

function WBP_MyOverlay:WidgetControllerSet() end

---@param Row FUIWidgetRow
function WBP_MyOverlay:MessageWidgetRowDelegate_Event(Row) end

function WBP_MyOverlay:OnAttributeButtonClicked() end

function WBP_MyOverlay:AttributeMenuButtonClicked() end

function WBP_MyOverlay:AttributeMenuClosed_Event() end

---@param NewValue integer
function WBP_MyOverlay:OnPlayerLevelChangedDelegate_Event(NewValue) end

function WBP_MyOverlay:OnSpellMenuButtonClicked() end

WBP_MyOverlay["Spell Menu Button Clicked"] = function(self) end

function WBP_MyOverlay:SpellMenuClosed_Event() end

