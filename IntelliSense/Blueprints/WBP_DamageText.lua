---@class WBP_DamageText : UUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public HitMessageAnim UWidgetAnimation
---@field public DamageAnim UWidgetAnimation
---@field public Text_Damage UTextBlock
---@field public Text_HitMessage UTextBlock
local WBP_DamageText = {}

---@param IsBlock boolean
---@param IsCrit boolean
---@param Message string @[out] 
function WBP_DamageText:GetHitMessageBasedOnBlockAndCrit(IsBlock, IsCrit, Message) end

---@param IsBlock boolean
---@param IsCrit boolean
---@param Color FSlateColor @[out] 
function WBP_DamageText:GetColorBasedOnBlockAndCrit(IsBlock, IsCrit, Color) end

---@param Damage number
---@param BlockedHit boolean
---@param CriticalHit boolean
function WBP_DamageText:UpdateDamageText(Damage, BlockedHit, CriticalHit) end

---Called after the underlying slate widget is constructed.  Depending on how the slate object is used
---this event may be called multiple times due to adding and removing from the hierarchy.
---If you need a true called-once-when-created event, use OnInitialized.
function WBP_DamageText:Construct() end

