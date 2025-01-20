---@class WBP_AttributeMenu : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public CloseButton WBP_Button_C
---@field public Row_ArcaneResistance WBP_TextValueRow_C
---@field public Row_Armor WBP_TextValueRow_C
---@field public Row_ArmorPenetration WBP_TextValueRow_C
---@field public Row_BlockChance WBP_TextValueRow_C
---@field public Row_CriticalHitChance WBP_TextValueRow_C
---@field public Row_CriticalHitDamage WBP_TextValueRow_C
---@field public Row_CriticalHitResistance WBP_TextValueRow_C
---@field public Row_FireResistance WBP_TextValueRow_C
---@field public Row_HealthRegeneration WBP_TextValueRow_C
---@field public Row_Intelligence WBP_TextValueButtonRow_C
---@field public Row_LightningResistance WBP_TextValueRow_C
---@field public Row_ManaRegeneration WBP_TextValueRow_C
---@field public Row_MaxHealth WBP_TextValueRow_C
---@field public Row_MaxMana WBP_TextValueRow_C
---@field public Row_PhysicalResistance WBP_TextValueRow_C
---@field public Row_Resilience WBP_TextValueButtonRow_C
---@field public Row_Strength WBP_TextValueButtonRow_C
---@field public Row_Vigor WBP_TextValueButtonRow_C
---@field public WBP_AttributePointsRow WBP_AttributePointsRow_C
---@field public AttributeMenuClosed MulticastDelegate
---@field public AttributeMenuWidgetController UAttributeMenuWidgetController
local WBP_AttributeMenu = {}

---@param AttributePoints integer
function WBP_AttributeMenu:SetButtonsEnabled(AttributePoints) end

function WBP_AttributeMenu:SetResistanceAttributeTags() end

function WBP_AttributeMenu:SetSecondaryAttributeTags() end

function WBP_AttributeMenu:SetPrimaryAttributeTags() end

function WBP_AttributeMenu:SetAttributeTags() end

---Called after the underlying slate widget is constructed.  Depending on how the slate object is used
---this event may be called multiple times due to adding and removing from the hierarchy.
---If you need a true called-once-when-created event, use OnInitialized.
function WBP_AttributeMenu:Construct() end

function WBP_AttributeMenu:OnClicked_Event() end

---Called when a widget is no longer referenced causing the slate resource to destroyed.  Just like
---Construct this event can be called multiple times.
function WBP_AttributeMenu:Destruct() end

---@param NewValue integer
function WBP_AttributeMenu:AttributePointsChanged(NewValue) end

function WBP_AttributeMenu:AttributeMenuClosed__DelegateSignature() end

