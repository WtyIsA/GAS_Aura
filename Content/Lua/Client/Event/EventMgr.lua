
---@class EventMgr
EventMgr = {
    _instance = nil,
    _listeners = nil
}
EventMgr.__index = EventMgr

EventMgr.EnterBsFinish = "EventMgr.EnterBsFinish"
EventMgr.OnRefreshCurFlashIndex = "EventMgr.OnRefreshCurFlashIndex"
EventMgr.OnRefreshFlashList = "EventMgr.OnRefreshFlashList"
EventMgr.OnCloseAddInterFace = "EventMgr.OnCloseAddInterFace"
EventMgr.DsNetFailureEvent = "EventMgr.OnDsNetFailureEventKey"

function EventMgr:new()
    local t = {}
    setmetatable(t, EventMgr)
    return t
end

function EventMgr:Instance()
    if EventMgr._instance == nil then
        EventMgr._instance = EventMgr:new()
        EventMgr._listeners = HashMap:new()
    end
    return EventMgr._instance
end



function EventMgr:Clear()
    if self._listeners then
        self._listeners:clear()
    end
end

---@private
function EventMgr.ProcessEvent(v, ...)
    if(v.p)then
        if(v.ret)then
            v.f(v.p, v.ret, ...)
        else
            v.f(v.p, ...)
        end
    else
        if(v.ret)then
            v.f(v.ret, ...)
        else
            v.f(...)
        end
    end
end

---@private
function EventMgr.PrintTraceback()
    print(debug.traceback())
end

function EventMgr:TriggerEvent(key, param)
    if not self._listeners:contains(key) then
        return
    end
    local listeners = self._listeners:get(key)
    local size = listeners:size()
    for i=1, size do
        if(i <= listeners:size())then
            local v = listeners:get(i);
            xpcall(EventMgr.ProcessEvent , EventMgr.PrintTraceback,v,param)
        end
    end
end

function EventMgr:AddEventListener(key, func, param, inputparam)
    ---@type ArrayList
    local listeners = self._listeners:get(key)

    if listeners == nil then
        listeners = ArrayList:new()
        self._listeners:add(key,listeners)
    end

    local size = listeners:size()
    for i=1, size do
        if(listeners:get(i).f == func and listeners:get(i).p == param)then
            return;
        end
    end
    listeners:add({ f = func, p = param, ret = inputparam })
end

function EventMgr:RemoveEventListener(key, func)
    ---@type ArrayList
    local listeners = self._listeners:get(key)
    if nil == listeners then
        return
    end
    local size = listeners:size()
    for i= size, 1, -1 do
        if(listeners:get(i).f == func)then
            listeners:removeAt(i);
            if(listeners:size() == 0)then
                self._listeners:remove(key);
            end
        end
    end
end

function EventMgr:ClearEventListener(Key)
    self._listeners:remove(Key)
end



function EventMgr:TriggerEventMultParam(key, ...)
    if not self._listeners:contains(key) then
        return
    end
    ---@type ArrayList
    local listeners = self._listeners:get(key)
    local size = listeners:size()
    for i=1, size do
        local v = listeners:get(i);
        xpcall(EventMgr.ProcessEvent , EventMgr.PrintTraceback,v,...)
    end

end