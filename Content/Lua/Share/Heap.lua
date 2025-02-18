---@class Heap
---@field data ArrayList
---@field value2Index HashMap
require("HashMap")
require("ArrayList")
Heap = {}
Heap.__index = Heap

---@return Heap
function Heap:new(isMinHeap)
    local o = {}
    o.data = ArrayList:new()
    o.value2Index = HashMap:new()
    if(isMinHeap == nil) then
        isMinHeap = true
    end
    o.isMinHeap = isMinHeap
    setmetatable(o,Heap)
    return o
end

---@return number
function Heap:size()
    return self.data:size()
end

---@private
---@param value any
---@return number
function Heap:indexOf(value)
    local index = self.value2Index:get(value)
    return index or -1
end

---@param value any
function Heap:add(value)
    if(self.value2Index:contains(value)) then
        return false
    end
    self.data:add(value)
    if(self.data:size() > 1) then
        self:siftUp(self.data:size(),value)
    else
        self.value2Index:add(value,1)
    end
    return true
end

---@param value any
---@return boolean
function Heap:remove(value)
    local index = self:indexOf(value)
    if(index < 0) then
        return false
    else
        self:removeAt(index)
        return true
    end
end

---@private
---@param index number
function Heap:removeAt(index)
    local size = self.data:size()
    if(index == size) then
        self.value2Index:remove(self.data:removeAt(index))
    else
        self.value2Index:remove(self.data:get(index))
        local value = self.data:removeAt(size)
        self:siftDown(index,value)
        if(self.data:get(index) == value) then
            self:siftUp(index,value)
        end
    end
end

---@private
---@param index number
---@param value any
function Heap:siftUp(index, value)
    local isMinHeap = self.isMinHeap
    while(index > 1)
    do
        local pIndex = index // 2
        local parent = self.data:get(pIndex)
        if((isMinHeap == true and value >= parent) or (isMinHeap == false and value <= parent )) then
            break
        end
        self.data:set(index,parent)
        self.value2Index:add(parent,index)
        index = pIndex
    end
    self.data:set(index,value)
    self.value2Index:add(value,index)
end

---@param value any
function Heap:siftDownByValue( value)
    local index = self:indexOf(value)
    if(index > 0) then
        self:siftDown(index,value)
    end
end

---@private
---@param index number
---@param value any
function Heap:siftDown(index, value)
    local size = self.data:size()
    local mid = size // 2
    local isMinHeap = self.isMinHeap
    while(index <= mid)
    do
        local lIndex = index * 2
        local child = self.data:get(lIndex)
        local rIndex = lIndex + 1
        if(rIndex <= size) then
            local rChild = self.data:get(rIndex)
            if((isMinHeap == true and rChild < child) or (isMinHeap == false and rChild > child )) then
                child = rChild
                lIndex = rIndex
            end
        end
        if((isMinHeap == true and value <= child) or (isMinHeap == false and value >= child )) then
            break
        end
        self.data:set(index,child)
        self.value2Index:add(child,index)
        index = lIndex
    end
    self.data:set(index,value)
    self.value2Index:add(value,index)
end

---@return any
function Heap:peek()
    if(self.data:size() == 0) then
        return nil
    end
    return self.data:get(1)
end