---@class WBP_TextValueRow : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public NamedSlot_50 UNamedSlot
---@field public SizeBox_Root USizeBox
---@field public TextBlock_Label UTextBlock
---@field public WBP_FramedValue WBP_FramedValue_C
---@field public BoxWidth number
---@field public BoxHeight number
---@field public AttributeTag FGameplayTag
local WBP_TextValueRow = {}

---@param NewValue integer
function WBP_TextValueRow:SetNumericalValueInt(NewValue) end

---@param LabelText string
function WBP_TextValueRow:SetLabelText(LabelText) end

---Called by both the game and the editor.  Allows users to run initial setup for their widgets to better preview
---the setup in the designer and since generally that same setup code is required at runtime, it's called there
---as well.
---**WARNING**
---This is intended purely for cosmetic updates using locally owned data, you can not safely access any game related
---state, if you call something that doesn't expect to be run at editor time, you may crash the editor.
---In the event you save the asset with blueprint code that causes a crash on evaluation.  You can turn off
---PreConstruct evaluation in the Widget Designer settings in the Editor Preferences.
---@param IsDesignTime boolean
function WBP_TextValueRow:PreConstruct(IsDesignTime) end

---Called after the underlying slate widget is constructed.  Depending on how the slate object is used
---this event may be called multiple times due to adding and removing from the hierarchy.
---If you need a true called-once-when-created event, use OnInitialized.
function WBP_TextValueRow:Construct() end

---@param Info FAuraAttributeInfo
function WBP_TextValueRow:AttributeInfoDelegate_Event(Info) end

