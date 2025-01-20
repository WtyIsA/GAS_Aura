---@class WBP_SpellGlobe : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public Image_Background UImage
---@field public Image_Glass UImage
---@field public Image_Ring UImage
---@field public Image_SpellIcon UImage
---@field public SizeBox_Root USizeBox
---@field public Text_Cooldown UTextBlock
---@field public BoxWidth number
---@field public BoxHeight number
---@field public RingBrush FSlateBrush
---@field public GlassPadding number
---@field public SpellIconBrush FSlateBrush
---@field public TransparentBrush FSlateBrush
---@field public BPOverlayWidgetController BP_OverlayWidgetController_C
---@field public InputTag FGameplayTag
---@field public WaitCooldownChangeTask UWaitCooldownChange
---@field public CooldownTag FGameplayTag
---@field public TimeRemaining number
---@field public CooldownTint number
---@field public TimerFrequency number
---@field public CooldownTimerHandle FTimerHandle
local WBP_SpellGlobe = {}

function WBP_SpellGlobe:HideCooldownText() end

function WBP_SpellGlobe:UpdateCooldownTimer() end

function WBP_SpellGlobe:SetDefaultState() end

function WBP_SpellGlobe:SetCooldownState() end

---@param AuraAbilityInfo FAuraAbilityInfo
function WBP_SpellGlobe:ReceiveAbilityInfo(AuraAbilityInfo) end

---@param IconBrush FSlateBrush
---@param BackgroundBrush FSlateBrush
function WBP_SpellGlobe:SetIconAndBackground(IconBrush, BackgroundBrush) end

function WBP_SpellGlobe:ClearGlobe() end

---@param Tint number
function WBP_SpellGlobe:SetBackgroundTint(Tint) end

function WBP_SpellGlobe:UpdateSpellIconBrush() end

function WBP_SpellGlobe:UpdateGlobePadding() end

function WBP_SpellGlobe:UpdateRingBrush() end

function WBP_SpellGlobe:UpdateBoxSize() end

---@param TimeRemaining number
function WBP_SpellGlobe:CooldownEnd_48987CF5481284E4828E27B4BC28C2F7(TimeRemaining) end

---@param TimeRemaining number
function WBP_SpellGlobe:CooldownStart_48987CF5481284E4828E27B4BC28C2F7(TimeRemaining) end

function WBP_SpellGlobe:WidgetControllerSet() end

---@param Info FAuraAbilityInfo
function WBP_SpellGlobe:AbilityInfoDelegate_Event(Info) end

function WBP_SpellGlobe:UpdateTimer() end

---Called when a widget is no longer referenced causing the slate resource to destroyed.  Just like
---Construct this event can be called multiple times.
function WBP_SpellGlobe:Destruct() end

---Called by both the game and the editor.  Allows users to run initial setup for their widgets to better preview
---the setup in the designer and since generally that same setup code is required at runtime, it's called there
---as well.
---**WARNING**
---This is intended purely for cosmetic updates using locally owned data, you can not safely access any game related
---state, if you call something that doesn't expect to be run at editor time, you may crash the editor.
---In the event you save the asset with blueprint code that causes a crash on evaluation.  You can turn off
---PreConstruct evaluation in the Widget Designer settings in the Editor Preferences.
---@param IsDesignTime boolean
function WBP_SpellGlobe:PreConstruct(IsDesignTime) end

