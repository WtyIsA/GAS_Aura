---@class WBP_Button : UAuraUserWidget
---@field public UberGraphFrame FPointerToUberGraphFrame
---@field public Button UButton
---@field public Image_Border UImage
---@field public SizeBox_Root USizeBox
---@field public Text_ButtonTitle UTextBlock
---@field public BoxWidth number
---@field public BoxHeight number
---@field public BorderBrush FSlateBrush
---@field public ButtonNormalBrush FSlateBrush
---@field public ButtonHoveredBrush FSlateBrush
---@field public ButtonPressedBrush FSlateBrush
---@field public ButtonDisabledBrush FSlateBrush
---@field public ButtonText string
---@field public Font Family UObject
---@field public Outline Size integer
---@field public FontSize integer
---@field public Letter Spacing integer
---@field public OnClickedSound USoundBase
---@field public OnHoveredSound USoundBase
local WBP_Button = {}

function WBP_Button:UpdateText() end

function WBP_Button:UpdateButtonBrushes() end

function WBP_Button:UpdateBorderBrush() end

function WBP_Button:UpdateBoxSize() end

---Called by both the game and the editor.  Allows users to run initial setup for their widgets to better preview
---the setup in the designer and since generally that same setup code is required at runtime, it's called there
---as well.
---**WARNING**
---This is intended purely for cosmetic updates using locally owned data, you can not safely access any game related
---state, if you call something that doesn't expect to be run at editor time, you may crash the editor.
---In the event you save the asset with blueprint code that causes a crash on evaluation.  You can turn off
---PreConstruct evaluation in the Widget Designer settings in the Editor Preferences.
---@param IsDesignTime boolean
function WBP_Button:PreConstruct(IsDesignTime) end

function WBP_Button:BndEvt__WBP_Button_Button_K2Node_ComponentBoundEvent_5_OnButtonClickedEvent__DelegateSignature() end

function WBP_Button:BndEvt__WBP_Button_Button_K2Node_ComponentBoundEvent_6_OnButtonHoverEvent__DelegateSignature() end

