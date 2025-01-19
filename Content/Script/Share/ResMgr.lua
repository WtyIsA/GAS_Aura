
require "UnLua"

---@class ResMgr
local ResMgr = Class()

---@type HashMap | table<string, ArrayList>
local _resCallBack = nil;
function ResMgr:Initialize(Initializer)    
end

function ResMgr:Init()
    require("HashMap")
    require("ArrayList")
    _resCallBack = HashMap:new();
end

function ResMgr:Load(path)
    local o = UE4.UObject.Load(path);
    return o;
end

function ResMgr:LoadResCallback(object, path,co)
    if(co ~= nil)then
        coroutine.resume(co,  object);
    end
end

---@param path string
---@param callbackObj UUserWidget 加载成功回调函数的对象
---@param callbackFun function  加载成功回调函数地址,两个参数， UObject, path
---@param co any: thread coroutine.create返回值, uwidget,object等 
function ResMgr:AsyncLoad(path, callbackObj, callbackFun, co, isMatchSize)   
    
    local tempObject = self:AsyncLoadRes(path, {Global.g_luaMgr, function(t, obj, resPath)
        if(callbackObj ~= nil and callbackFun ~= nil)then
            callbackFun(callbackObj, obj, resPath, co, isMatchSize);
        end
    end});
    
end

function ResMgr:OnCompleteBroadcast(obj, resPath)  
    ---@type ArrayList
    local cbs = _resCallBack:get(resPath);
    if(cbs == nil)then
        return
    end
    local nSize = cbs:size();
    for i=1, nSize,1 do
        local item = cbs:get(i);
        item.callbackFun(item.cbObj, obj, resPath, item.co, item.isMatchSize);
    end
    _resCallBack:remove(resPath);
end

return ResMgr;