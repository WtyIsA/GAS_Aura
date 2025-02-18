---@class Tick
---@field nextTickTime number
---@field tickTime number
---@field tickObject table
---@field tickFunc function
---@field id number
Tick = {}
Tick.__index = Tick
Tick.MaxCacheNum = 10000
Tick.NextId = 0

local TickList = ArrayList:new()

local function OnError(err)
    print("Tick Error:", err)
    print(debug.traceback())
end

function Tick:new(tickObject,tickFunc,nextTickTime)
    local o
    if(TickList:size() > 0) then
        o = TickList:removeAt(TickList:size())
    else
        o = {}
        setmetatable(o,Tick)
    end
    Tick.NextId = Tick.NextId + 1
    o.id = Tick.NextId
    o.tickObject = tickObject
    o.tickFunc = tickFunc
    o.nextTickTime = nextTickTime
    o.tickTime = Utils.GetTimeInMill()
    return o
end

---@param time number
---@param fakeTime number
function Tick:GetTickDeltaTime(time,fakeTime)
    local deltaTime = time - self.tickTime
    if(deltaTime < 0) then
        self.tickTime = time
        deltaTime = 0
    else
        self.tickTime = fakeTime
    end
    return deltaTime
end

---@return number
function Tick:GetTickTime()
    return self.nextTickTime
end

---@param nextTickTime number
function Tick:SetNextTickTime(nextTickTime)
    self.nextTickTime = nextTickTime
end

---@private
---@param tick Tick
function Tick:__eq(tick)
    return self.tickObject == tick.tickObject and self.tickFunc == tick.tickFunc
end

---@private
---@param tick Tick
function Tick:__lt(tick)
    return (self.nextTickTime - tick.nextTickTime) < 0
end

---@private
---@param tick Tick
function Tick:__le(tick)
    return (self.nextTickTime - tick.nextTickTime) <= 0
end

---@param deltaSeconds number
---@param time number
---@return number
function Tick:OnTick(deltaSeconds,time)
    local s, r = xpcall(self.tickFunc, OnError, self.tickObject,deltaSeconds,time)
    if(r == nil or s == false) then
        return -1
    end
    return r
end

function Tick:Recycle()
    if(TickList:size() < Tick.MaxCacheNum) then
        TickList:add(self)
    end
end 