---@class WBP_ValueGlobe : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public Image_Background UImage
---@field public Image_Glass UImage
---@field public Image_Ring UImage
---@field public SizeBox_Root USizeBox
---@field public Text_Value UTextBlock
---@field public BoxWidth number
---@field public BoxHeight number
---@field public RingBrush FSlateBrush
---@field public GlassPadding number
---@field public BackgroundBrush FSlateBrush
---@field public TransparentBrush FSlateBrush
---@field public BPOverlayWidgetController BP_OverlayWidgetController_C
local WBP_ValueGlobe = {}

WBP_ValueGlobe["Update Background"] = function(self) end

---@param Tint number
function WBP_ValueGlobe:SetBackgroundTint(Tint) end

function WBP_ValueGlobe:UpdateGlobePadding() end

function WBP_ValueGlobe:UpdateRingBrush() end

function WBP_ValueGlobe:UpdateBoxSize() end

function WBP_ValueGlobe:WidgetControllerSet() end

---Called by both the game and the editor.  Allows users to run initial setup for their widgets to better preview
---the setup in the designer and since generally that same setup code is required at runtime, it's called there
---as well.
---**WARNING**
---This is intended purely for cosmetic updates using locally owned data, you can not safely access any game related
---state, if you call something that doesn't expect to be run at editor time, you may crash the editor.
---In the event you save the asset with blueprint code that causes a crash on evaluation.  You can turn off
---PreConstruct evaluation in the Widget Designer settings in the Editor Preferences.
---@param IsDesignTime boolean
function WBP_ValueGlobe:PreConstruct(IsDesignTime) end

---@param NewValue integer
function WBP_ValueGlobe:OnPlayerLevelChangedDelegate_Event(NewValue) end

