
---@class LinkedList
LinkedList =  {}
LinkedList.__index = LinkedList
LinkedList.Node = {}

function LinkedList.Node.new(val)
    local o = {}
    o.val = val
    o.next = nil
    return o
end

function LinkedList.Node:__tostring__()
    return 'LinkedListNode(' .. self.val .. ')'
end

---@return LinkedList
function LinkedList:new(values)
    local o = {}
    o._sentinel = LinkedList.Node.new()
    o._tail = o._sentinel
    o._size = 0
    setmetatable(o, LinkedList)
    if (values) then
        for _, v in ipairs(values) do
            o:add(v)
        end
    end
    return o
end

function LinkedList:head()
    return self._sentinel.next
end

function LinkedList:size()
    return self._size
end

function LinkedList:isEmpty()
    return self._size == 0
end

function LinkedList:clear()
    if (self._size > 0) then
        self._sentinel.next = nil
        self._tail = self._sentinel
        self._size = 0;
    end
end

function LinkedList:add(val, index)
    local node = LinkedList.Node.new(val)
    if index == nil then
        self._tail.next = node
        self._tail = node
    else
        self:assertValidIndex(index)
        local count = 1
        local prev = self._sentinel
        local curr = self:head()
        while count ~= index do
            prev = curr
            curr = curr.next
            count = count + 1
        end
        prev.next = node
        prev.next.next = curr
    end
    self._size = self._size + 1
    return self
end

function LinkedList:get(index)
    self:assertValidIndex(index)
    local count = 1
    local curr = self:head()
    while count ~= index do
        curr = curr.next
        count = count + 1
    end
    return curr.val
end

function LinkedList:set(index, val)
    self:assertValidIndex(index)
    local count = 1
    local curr = self:head()
    while count ~= index do
        curr = curr.next
        count = count + 1
    end
    curr.val = val
    return self
end

function LinkedList:remove(index)
    self:assertValidIndex(index)
    local count = 1
    local prev = self._sentinel
    local curr = self:head()
    while count ~= index do
        prev = curr
        curr = curr.next
        count = count + 1
    end
    prev.next = curr.next
    if curr == self._tail then self._tail = prev end
    self._size = self._size - 1
    return curr.val
end

function LinkedList:peek()
    if (self._sentinel.next) then
        return self._sentinel.next.val
    end
    return nil
end

function LinkedList:peekLast()
    if (self._tail ~= self._sentinel) then
        return self._tail.val
    end
    return nil
end

function LinkedList:swap(i, j)
    self:assertValidIndex(i)
    self:assertValidIndex(j)
    local count = 1
    local prev = self._sentinel
    local curr = self:head()
    local currI, prevI, currJ, prevJ
    while count <= math.max(i, j) do
        if count == i then
            prevI = prev
            currI = curr
        end
        if count == j then
            prevJ = prev
            currJ = curr
        end
        count = count + 1
        prev = curr
        curr = curr.next
    end
    assert(prevI)
    assert(currI)
    assert(prevJ)
    assert(currJ)
    prevI.next = currJ
    prevJ.next = currI
    local temp = currI.next
    currI.next = currJ.next
    currJ.next = temp
    return self
end

function LinkedList:equals(another)
    if self:size() ~= another:size() then return false end
    local curr = self:head()
    local currAnother = another:head()
    while curr ~= nil do
        if curr.val ~= currAnother.val then return false end
        curr = curr.next
        currAnother = currAnother.next
    end
    return true
end

--- Asserts that index is inside the list.
-- @arg {int} index - index to check
function LinkedList:assertValidIndex(index)
    if index < 1 or index > self._size then
        error('index  is out of bounds for array of size ');
    end
end

function LinkedList:totable()
    local tab = {}
    local curr = self:head()
    while curr do
        table.insert(tab, curr.val)
        curr = curr.next
    end
    return tab
end

function LinkedList:getIndex(val)
    local count = 0
    local curr = self:head()
    while curr do
        count = count + 1;
        if curr.val == val then
            return count
        end
        curr = curr.next
    end
    return nil;
end

---@return Iterator
function LinkedList:iterator()
    local iter = Iterator:new(self);
    return iter;
end

---@class Iterator
---@field _Link LinkedList
---@field _Curr table
Iterator = {}
Iterator.__index = Iterator

---@return Iterator
---@param lickObj LinkedList
function Iterator:new(lickObj)
    local o = {}
    setmetatable(o, Iterator)
    o._Curr = lickObj:head();
    return o;
end

function Iterator:hasNext()
    if(self._Curr ~= nil)then
        return true;
    else
        return false
    end
end

function Iterator:next()
    local o = self._Curr.val;
    self._Curr = self._Curr.next;
    return o;
end

return LinkedList