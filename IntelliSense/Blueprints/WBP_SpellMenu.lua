---@class WBP_SpellMenu : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public Button_Equip WBP_WideButton_C
---@field public Button_SpendPoint WBP_WideButton_C
---@field public CloseButton WBP_Button_C
---@field public FramedValue_SpellPoints WBP_FramedValue_C
---@field public RichText_Description URichTextBlock
---@field public RichText_NextLevelDescription URichTextBlock
---@field public SizeBox_Root USizeBox
---@field public WBP_EquippedSpellRow WBP_EquippedSpellRow_C
---@field public WBP_OffensiveSpellTree WBP_OffensiveSpellTree_C
---@field public WBP_PassiveSpellTree WBP_PassiveSpellTree_C
---@field public AttributeMenuWidgetController UAttributeMenuWidgetController
---@field public SpellMenuClosed MulticastDelegate
---@field public BPSpellMenuWidgetController BP_SpellMenuWidgetController_C
local WBP_SpellMenu = {}

---@param AbilityType FGameplayTag
function WBP_SpellMenu:StopWaitingForEquip(AbilityType) end

---@param AbilityType FGameplayTag
function WBP_SpellMenu:OnWaitForEquip(AbilityType) end

---@param InString string
---@param InString2 string
function WBP_SpellMenu:SetDescriptions(InString, InString2) end

---@param SpendPointsEnabled boolean
---@param EquipEnabled boolean
function WBP_SpellMenu:SetButtonsEnabled(SpendPointsEnabled, EquipEnabled) end

---@param Value integer
function WBP_SpellMenu:SpellPointsChanged(Value) end

---Called when a widget is no longer referenced causing the slate resource to destroyed.  Just like
---Construct this event can be called multiple times.
function WBP_SpellMenu:Destruct() end

---@param NewValue integer
function WBP_SpellMenu:SpellPointsChanged_Event(NewValue) end

function WBP_SpellMenu:OnOffensiveSpellGlobeSelected_Event() end

function WBP_SpellMenu:OnPassiveSpellGlobeSelected_Event() end

---Called by both the game and the editor.  Allows users to run initial setup for their widgets to better preview
---the setup in the designer and since generally that same setup code is required at runtime, it's called there
---as well.
---**WARNING**
---This is intended purely for cosmetic updates using locally owned data, you can not safely access any game related
---state, if you call something that doesn't expect to be run at editor time, you may crash the editor.
---In the event you save the asset with blueprint code that causes a crash on evaluation.  You can turn off
---PreConstruct evaluation in the Widget Designer settings in the Editor Preferences.
---@param IsDesignTime boolean
function WBP_SpellMenu:PreConstruct(IsDesignTime) end

---@param bSpendPointsButtonEnabled boolean
---@param bEquipButtonEnabled boolean
---@param DescriptionString string
---@param NextLevelDescriptionString string
function WBP_SpellMenu:SpellGlobeSelectedDelegate_Event(bSpendPointsButtonEnabled, bEquipButtonEnabled, DescriptionString, NextLevelDescriptionString) end

function WBP_SpellMenu:OnClicked_Event_0() end

function WBP_SpellMenu:OnClicked_Event() end

---@param AbilityType FGameplayTag
function WBP_SpellMenu:WaitForEquipDelegate_Event(AbilityType) end

function WBP_SpellMenu:OnClicked_Event_1() end

---Called after the underlying slate widget is constructed.  Depending on how the slate object is used
---this event may be called multiple times due to adding and removing from the hierarchy.
---If you need a true called-once-when-created event, use OnInitialized.
function WBP_SpellMenu:Construct() end

---@param AbilityType FGameplayTag
function WBP_SpellMenu:StopWaitingForEquipDelegate_Event(AbilityType) end

function WBP_SpellMenu:SpellMenuClosed__DelegateSignature() end

