
---@class TickList
TickList = {}

---@type ArrayList
local listenersObj = ArrayList:new()

---@type ArrayList
local listenersFun = ArrayList:new()

local toRemoveSet = HashSet:new()

---@alias Handler fun(msg : Connect) : void
---@param handler Handler
---@return void
function TickList:push(obj, handler)
    for _, v in pairs(toRemoveSet) do
        if(v == obj)then
            toRemoveSet:remove(v);
            break;
        end
    end
    for n=1,listenersObj:size(),1 do
        if (listenersObj:get(n) == obj) then
            return
        end
    end
    listenersFun:add(handler);
    listenersObj:add(obj)
end

---@param handler Handler
---@return void
function TickList:Remove(obj)
    toRemoveSet:add(obj);
end

function TickList:Tick(DeltaSeconds)
    self:CheckRemoveList();
    for n=1,listenersFun:size(),1 do
        if(listenersObj:get(n) ~= nil)then
            listenersFun:get(n)(listenersObj:get(n),DeltaSeconds)
        else
            print("listeners obj is nil at:"..n);
        end

    end
end

---@return number 数组长度
function TickList:GetSize()
    return listenersFun:size()
end

function TickList:Clear()
    listenersFun:clear();
    listenersObj:clear();
end

function TickList:CheckRemoveList()
    for _, v in pairs(toRemoveSet) do
        for n=1,listenersObj:size(),1 do
            if (listenersObj:get(n) == v) then
                listenersObj:removeAt(n)
                listenersFun:removeAt(n)
                break;
            end
        end
    end
    toRemoveSet:clear();
end 