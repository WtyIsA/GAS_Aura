---@class WBP_EffectMessage : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public MessageAnimation UWidgetAnimation
---@field public Image_Icon UImage
---@field public Text_Message UTextBlock
---@field public ImageSize FVector2D
local WBP_EffectMessage = {}

---@param Image UTexture2D
---@param Text string
function WBP_EffectMessage:SetImageAndText(Image, Text) end

function WBP_EffectMessage:DestroyDelay() end

