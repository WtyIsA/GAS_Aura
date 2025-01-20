---@class WBP_EquippedRow_Button : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public SelectAnimation UWidgetAnimation
---@field public Button_Ring UButton
---@field public Image_Background UImage
---@field public Image_Glass UImage
---@field public Image_Icon UImage
---@field public SizeBox_Root USizeBox
---@field public BoxWidth number
---@field public BoxHeight number
---@field public RingBrush FSlateBrush
---@field public GlassPadding number
---@field public BackgroundBrush FSlateBrush
---@field public TransparentBrush FSlateBrush
---@field public BPSpellMenuWidgetController BP_SpellMenuWidgetController_C
---@field public InputTag FGameplayTag
---@field public AbilityType FGameplayTag
local WBP_EquippedRow_Button = {}

---@param AuraAbilityInfo FAuraAbilityInfo
function WBP_EquippedRow_Button:ReceiveAbilityInfo(AuraAbilityInfo) end

function WBP_EquippedRow_Button:UpdateBackground() end

function WBP_EquippedRow_Button:ClearGlobe() end

---@param Tint number
function WBP_EquippedRow_Button:SetBackgroundTint(Tint) end

function WBP_EquippedRow_Button:UpdateGlobePadding() end

function WBP_EquippedRow_Button:UpdateBoxSize() end

function WBP_EquippedRow_Button:WidgetControllerSet() end

---Called by both the game and the editor.  Allows users to run initial setup for their widgets to better preview
---the setup in the designer and since generally that same setup code is required at runtime, it's called there
---as well.
---**WARNING**
---This is intended purely for cosmetic updates using locally owned data, you can not safely access any game related
---state, if you call something that doesn't expect to be run at editor time, you may crash the editor.
---In the event you save the asset with blueprint code that causes a crash on evaluation.  You can turn off
---PreConstruct evaluation in the Widget Designer settings in the Editor Preferences.
---@param IsDesignTime boolean
function WBP_EquippedRow_Button:PreConstruct(IsDesignTime) end

---@param Info FAuraAbilityInfo
function WBP_EquippedRow_Button:AbilityInfoDelegate_Event(Info) end

function WBP_EquippedRow_Button:OnClicked_Event() end

---Called when a widget is no longer referenced causing the slate resource to destroyed.  Just like
---Construct this event can be called multiple times.
function WBP_EquippedRow_Button:Destruct() end

