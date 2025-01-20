---@class WBP_PictureFrame : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public Image_Background UImage
---@field public Image_Glass UImage
---@field public SizeBox_Root USizeBox
---@field public BoxWidth number
---@field public BoxHeight number
---@field public BackgroundBrush FSlateBrush
---@field public ProgressBarBrush FSlateBrush
---@field public GlobePadding number
---@field public GlassBrush FSlateBrush
---@field public GhostPercentTarget number
---@field public GhostProgressBarBrush FSlateBrush
---@field public GhostInterpSpeed number
---@field public GlobeInitialized boolean
local WBP_PictureFrame = {}

function WBP_PictureFrame:UpdateGlassPadding() end

function WBP_PictureFrame:UpdateGlassBrush() end

function WBP_PictureFrame:UpdateBackgroundBrush() end

function WBP_PictureFrame:UpdateBoxSize() end

---Called by both the game and the editor.  Allows users to run initial setup for their widgets to better preview
---the setup in the designer and since generally that same setup code is required at runtime, it's called there
---as well.
---**WARNING**
---This is intended purely for cosmetic updates using locally owned data, you can not safely access any game related
---state, if you call something that doesn't expect to be run at editor time, you may crash the editor.
---In the event you save the asset with blueprint code that causes a crash on evaluation.  You can turn off
---PreConstruct evaluation in the Widget Designer settings in the Editor Preferences.
---@param IsDesignTime boolean
function WBP_PictureFrame:PreConstruct(IsDesignTime) end

