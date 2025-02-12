---@class WBP_SpellGlobe_Button : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public SelectAnimation UWidgetAnimation
---@field public Button_Ring UButton
---@field public Image_Background UImage
---@field public Image_Glass UImage
---@field public Image_Icon UImage
---@field public Image_Selection UImage
---@field public SizeBox_Root USizeBox
---@field public BoxWidth number
---@field public BoxHeight number
---@field public RingBrush FSlateBrush
---@field public GlassPadding number
---@field public BackgroundBrush FSlateBrush
---@field public TransparentBrush FSlateBrush
---@field public AbilityTag FGameplayTag
---@field public BPSpellMenuWidgetController BP_SpellMenuWidgetController_C
---@field public OnSpellGlobeSelected MulticastDelegate
---@field public Selected boolean
local WBP_SpellGlobe_Button = {}

---@param AbilityTag FGameplayTag
function WBP_SpellGlobe_Button:OnSpellGlobeReassigned(AbilityTag) end

function WBP_SpellGlobe_Button:Deselect() end

function WBP_SpellGlobe_Button:Select() end

---@param Texture UTexture2D
function WBP_SpellGlobe_Button:SetGlobeEligible(Texture) end

---@param Texture UTexture2D
---@param Material UMaterialInterface
function WBP_SpellGlobe_Button:SetGlobeEquippedOrUnlocked(Texture, Material) end

function WBP_SpellGlobe_Button:SetGlobeLocked() end

---@param AuraAbilityInfo FAuraAbilityInfo
function WBP_SpellGlobe_Button:ReceiveAbilityInfo(AuraAbilityInfo) end

function WBP_SpellGlobe_Button:UpdateBackground() end

function WBP_SpellGlobe_Button:ClearGlobe() end

---@param Tint number
function WBP_SpellGlobe_Button:SetBackgroundTint(Tint) end

function WBP_SpellGlobe_Button:UpdateGlobePadding() end

function WBP_SpellGlobe_Button:UpdateBoxSize() end

---Called by both the game and the editor.  Allows users to run initial setup for their widgets to better preview
---the setup in the designer and since generally that same setup code is required at runtime, it's called there
---as well.
---**WARNING**
---This is intended purely for cosmetic updates using locally owned data, you can not safely access any game related
---state, if you call something that doesn't expect to be run at editor time, you may crash the editor.
---In the event you save the asset with blueprint code that causes a crash on evaluation.  You can turn off
---PreConstruct evaluation in the Widget Designer settings in the Editor Preferences.
---@param IsDesignTime boolean
function WBP_SpellGlobe_Button:PreConstruct(IsDesignTime) end

function WBP_SpellGlobe_Button:WidgetControllerSet() end

---@param Info FAuraAbilityInfo
function WBP_SpellGlobe_Button:AbilityInfoDelegate_Event(Info) end

function WBP_SpellGlobe_Button:BndEvt__WBP_SpellGlobe_Button_Button_Ring_K2Node_ComponentBoundEvent_0_OnButtonClickedEvent__DelegateSignature() end

---@param AbilityTag FGameplayTag
function WBP_SpellGlobe_Button:SpellGlobeReassignedDelegate_Event(AbilityTag) end

---Called when a widget is no longer referenced causing the slate resource to destroyed.  Just like
---Construct this event can be called multiple times.
function WBP_SpellGlobe_Button:Destruct() end

---@param SelectedGlobe WBP_SpellGlobe_Button_C
function WBP_SpellGlobe_Button:OnSpellGlobeSelected__DelegateSignature(SelectedGlobe) end

