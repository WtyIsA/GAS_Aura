require("UI.UIWidgetBase")

---@type WBP_Rotating_loading|UIWidgetBase
RotatingLoadingPanel = Class("UI.UIWidgetBase")

--function Rotating_loadingPanel:OnInit()

--end

function RotatingLoadingPanel:OnShow(fromPanelName, checkHideFunc)
    self.checkHideFunc = checkHideFunc
    TickList:push(self, RotatingLoadingPanel.Tick);
end

function RotatingLoadingPanel:OnHide()
    TickList:Remove(self);
end

--function Rotating_loadingPanel:OnDestroy()
--end


function RotatingLoadingPanel:Tick(InDeltaTime)
    local angle = self.m_circle:GetRenderTransformAngle();
    angle = angle + 540*InDeltaTime;
    if(angle >= 360)then
        angle = angle - 360;
    end
    self.m_circle:SetRenderTransformAngle(angle);

    if self.checkHideFunc and self.checkHideFunc() then
        self:Hide()
    end
end


return RotatingLoadingPanel