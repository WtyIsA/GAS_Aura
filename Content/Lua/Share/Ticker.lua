---@class Ticker
---@field tickListeners Heap
---@field time number
---@field advanceTime number
---@field fakeTime number
---@field object2FuncList HashMap

require("Tick")
require("Heap")
require("Utils")
require("CachedCollections")

Ticker = {}
Ticker.__index = Ticker

function Ticker:new()
    local o = {}
    o.tickListeners = Heap:new()
    o.time = Utils.GetTimeInMill()
    o.fakeTime = o.time
    o.advanceTime = 0
    o.object2FuncList = HashMap:new()
    setmetatable(o,Ticker)
    return o
end

---@return boolean
function Ticker:RegisterTick(tickObject,tickFunction,delay)
    if(tickObject == nil or tickFunction == nil) then
        return false
    end
    local tickList = self.object2FuncList:get(tickObject)
    if(tickList == nil) then
        tickList = CachedCollections:New(ArrayList)
        self.object2FuncList:add(tickObject,tickList)
    else
        local size = tickList:size()
        for index=1,size do
            ---@type Tick
            local tick = tickList:get(index)
            if(tick.tickFunc == tickFunction) then
                tickList:removeAt(index)
                self.tickListeners:remove(tick)
                tick:Recycle()
                break
            end
        end
    end
    local tick = Tick:new(tickObject,tickFunction,Utils.GetTimeInMill() + delay + self.advanceTime)
    tickList:add(tick)
    return self.tickListeners:add(tick)
end

function Ticker:UnRegisterTick(tickObject,tickFunction)
    local tickList = self.object2FuncList:get(tickObject)
    if(tickList ~= nil) then
        local size = tickList:size()
        for index=1,size do
            ---@type Tick
            local tick = tickList:get(index)
            if(tick.tickFunc == tickFunction) then
                tickList:removeAt(index)
                if(tickList:size() == 0) then
                    local list = self.object2FuncList:remove(tickObject)
                    CachedCollections:Recycle(list)
                end
                self.tickListeners:remove(tick)
                tick:Recycle()
                return true
            end
        end
    end
    return false
end

function Ticker:ClearAllObjectTick(tickObject)
    local tickList = self.object2FuncList:get(tickObject)
    if (tickList ~= nil) then
        local size = tickList:size()
        for index = 1, size do
            ---@type Tick
            local tick = tickList:get(index)
            self.tickListeners:remove(tick)
            tick:Recycle()
        end
        tickList:clear()
        local list =  self.object2FuncList:remove(tickObject)
        CachedCollections:Recycle(list)
    end
end

function Ticker:OnTick(deltaSeconds)
    self.time = Utils.GetTimeInMill()
    self.fakeTime = self.time + self.advanceTime
    while(true)
    do
        ---@type Tick
        local tick = self.tickListeners:peek()
        if(tick == nil or tick:GetTickTime() > self.fakeTime) then
            break
        end
        local oldId = tick.id
        local nextTickTime = tick:OnTick(tick:GetTickDeltaTime(self.time,self.fakeTime),self.fakeTime)
        if(tick.id == oldId) then
            if(nextTickTime <= 0) then
                self:UnRegisterTick(tick.tickObject,tick.tickFunc)
            else
                tick:SetNextTickTime(self.fakeTime + nextTickTime)
                self.tickListeners:siftDownByValue(tick)
            end
        end
    end
end

function Ticker:Size()
    return self.tickListeners:size()
end

---@param time
function Ticker:Advance(time)
    self.advanceTime = self.advanceTime + time
end 