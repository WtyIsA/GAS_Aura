
---@class ClientResMgr
ClientResMgr = {}

local m_iconPath = "/Game/UI/UI_PC/IconAtlas/"
local m_uiTexturePath = "/Game/UI/UI_PC/"

function ClientResMgr:Init()
    if(PlatformMgr.nPlatformId == PlatformMgr.PC)then
        m_iconPath = "/Game/UI/UI_PC/IconAtlas/"
        m_uiTexturePath = "/Game/UI/UI_PC/"
    else
        m_iconPath = "/Game/UI/UI_Mobile/IconAtlas/"
        m_uiTexturePath = "/Game/UI/UI_Mobile/"
    end
end

---@private
function ClientResMgr:GetUITextureFullPath(path)
    local nPos = Utils.FindLastStrPos(path, "/");
    local name = "";
    if(nPos)then
        name = string.sub(path,nPos+1);
    else
        name = path;
    end
    local fullPath;
    if(string.sub(path, 1,5) == "/Game")then
        fullPath = path.."."..name
    else
        if(nPos)then
            if PlatformMgr.IsPc() then
                fullPath = m_uiTexturePath..string.sub(path,1,nPos)..name.."."..name
            else
                fullPath = m_uiTexturePath..string.sub(path,1,nPos).."m_"..name..".m_"..name
            end
        else
            if PlatformMgr.IsPc() then
                fullPath = m_uiTexturePath..path.."."..path;
            else
                fullPath = m_uiTexturePath.."m_".. path..".m_"..path;
            end
        end
    end
    return fullPath
end

---@private
function ClientResMgr:GetSpriteFullPath(path)
    local fullPath;
    if(PlatformMgr.nPlatformId == PlatformMgr.PC)then
        local nPos = Utils.FindLastStrPos(path, "/");
        if(nPos)then
            local name = string.sub(path,nPos+1);
            fullPath = m_iconPath..string.sub(path,1,nPos).."Frames/"..name.."."..name
        else
            fullPath = m_iconPath..path.."."..path;
        end
    else
        local nPos = Utils.FindLastStrPos(path, "/");
        if(nPos)then
            local name = string.sub(path,nPos+1);
            fullPath = m_iconPath..string.sub(path,1,nPos).."Frames/m_"..name..".m_"..name
        else
            fullPath = m_iconPath.."m_"..path..".m_"..path;
        end
    end
    return fullPath;
end

---@private
function ClientResMgr:GetImageFullPath(path)
    local fullPath;
    local bSprite = true;
    if(string.sub(path, 1, 7) == "Texture")then
        fullPath= self:GetUITextureFullPath(path);
        bSprite = false;
    else
        fullPath = self:GetSpriteFullPath(path);
    end
    return fullPath, bSprite;
end

---@alias 动态设置UImage的sprite图片
---@param image UImage
---@param path string 图片的路径
---@param isMatchSize boolean
---@param defaultImage UTexture2D
function ClientResMgr:SetUImageSpriteTexture(image, path, isMatchSize, defaultImage)
    if not image.Brush.ResourceObject then
        if defaultImage then
            image:SetBrushFromTexture(defaultImage,false)
        else
            image:SetBrushFromTexture(UIWidgetMgr.GetDefaultImage(),false)
        end
    end
    local fullPath, bSprite = self:GetImageFullPath(path);
    if (bSprite) then
        image:SetBrushFromSpriteAsync(fullPath, isMatchSize)
    else
        image:SetBrushFromTextureAsync(fullPath, isMatchSize)
    end
end

---@alias 动态设置UImage的UITexture图片
---@param image UImage
---@param path string 图片的路径
function ClientResMgr:SetUImageTexture(image, path)
    ClientResMgr:SetUImageSpriteTexture(image, path, false)
end

---@param img UImage
function ClientResMgr:SetUImageTextureAsync(img, path, isMatchSize)
    if isMatchSize then
        self:SetUImageSpriteTexture(img, path, isMatchSize)
    else
        local fullPath = self:GetImageFullPath(path);
        local opacity = img:GetRenderOpacity()
        img:SetOpacity(0)
        Global.g_resMgr:AsyncLoad(fullPath, self, function(_, uClass)
            img:SetOpacity(opacity)
            img:SetBrushFromTexture(uClass)
        end)
    end
end
