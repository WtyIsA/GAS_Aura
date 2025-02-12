---@class WBP_ProgressBar : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public ProgressBar_Front UProgressBar
---@field public ProgressBar_Ghost UProgressBar
---@field public ProgressBar_Max UProgressBar
---@field public SizeBox_Root USizeBox
---@field public BoxWidth number
---@field public BoxHeight number
---@field public FrontBarFillBrush FSlateBrush
---@field public GhostBarFillBrush FSlateBrush
---@field public GhostPercentTarget number
---@field public HideTimer FTimerHandle
---@field public ProgressBarVisible boolean
local WBP_ProgressBar = {}

---@param Visible boolean
function WBP_ProgressBar:SetBarVisibility(Visible) end

---@param DeltaTime number
function WBP_ProgressBar:InterpGhostBar(DeltaTime) end

---@param Value number
---@param MaxValue number
function WBP_ProgressBar:SetBarPercent(Value, MaxValue) end

function WBP_ProgressBar:UpdateGhostFillBrush() end

function WBP_ProgressBar:UpdateFrontFillBrush() end

function WBP_ProgressBar:UpdateBoxSize() end

---Called by both the game and the editor.  Allows users to run initial setup for their widgets to better preview
---the setup in the designer and since generally that same setup code is required at runtime, it's called there
---as well.
---**WARNING**
---This is intended purely for cosmetic updates using locally owned data, you can not safely access any game related
---state, if you call something that doesn't expect to be run at editor time, you may crash the editor.
---In the event you save the asset with blueprint code that causes a crash on evaluation.  You can turn off
---PreConstruct evaluation in the Widget Designer settings in the Editor Preferences.
---@param IsDesignTime boolean
function WBP_ProgressBar:PreConstruct(IsDesignTime) end

---Ticks this widget.  Override in derived classes, but always call the parent implementation.
---@param MyGeometry FGeometry
---@param InDeltaTime number
function WBP_ProgressBar:Tick(MyGeometry, InDeltaTime) end

---@param Target number
function WBP_ProgressBar:UpdateGhostInterpTarget(Target) end

function WBP_ProgressBar:HideProgressBar() end

---Called after the underlying slate widget is constructed.  Depending on how the slate object is used
---this event may be called multiple times due to adding and removing from the hierarchy.
---If you need a true called-once-when-created event, use OnInitialized.
function WBP_ProgressBar:Construct() end

