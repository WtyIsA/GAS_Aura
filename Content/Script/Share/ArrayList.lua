
---@class ArrayList
ArrayList = {}
ArrayList.__index = ArrayList

---@return ArrayList
function ArrayList:new(values)
    values = values or {}
    local o = {}
    o._data = values
    o._size = #values
    setmetatable(o, ArrayList)
    return o
end


---@param val any
---@param index number
---@return ArrayList
function ArrayList:add(val, index)
    if index == nil then
        self._data[self._size + 1] = val
    else
        self:assertValidIndex(index)
        for i = self._size, index, -1 do
            self._data[i + 1] = self._data[i]
        end
        self._data[index] = val
    end
    self._size = self._size + 1
    return self
end

---@param index number
---@return any
function ArrayList:get(index)
    self:assertValidIndex(index)
    return self._data[index]
end

---@param index number
---@param val any
---@return ArrayList
function ArrayList:set(index, val)
    self:assertValidIndex(index)
    self._data[index] = val
    return self
end

function ArrayList:indexOf(val)
    for i = 1, self._size do
        if (self._data[i] == val) then
            return i
        end
    end
    return -1
end

function ArrayList:lastIndexOf(val)
    for i = self._size , 1, -1 do
        if (self._data[i] == val) then
            return i
        end
    end
    return -1
end

---@param index number
---@return any
function ArrayList:removeAt(index)
    self:assertValidIndex(index)
    local val = self._data[index]
    for i = index + 1, self._size  do
        self._data[i - 1] = self._data[i]
    end
    self._data[self._size] = nil
    self._size = self._size - 1
    return val
end

function ArrayList:remove(val)
    local index = self:indexOf(val)
    if (index > 0) then
        for i = index + 1, self._size  do
            self._data[i - 1] = self._data[i]
        end
        self._data[self._size] = nil
        self._size = self._size - 1
    end
end

function ArrayList:equals(another)
    if another == nil then
        return false
    end
    if self:size() ~= another:size() then return false end
    for i = 1, self:size() do
        if self:get(i) ~= another:get(i) then return false end
    end
    return true
end

---@param i number
---@param j number
---@return ArrayList
function ArrayList:swap(i, j)
    self:assertValidIndex(i)
    self:assertValidIndex(j)
    temp = self._data[i]
    self._data[i] = self._data[j]
    self._data[j] = temp
    return self
end

---@param val any
function ArrayList:push(val)
    self:add(val);
end

---@return any
function ArrayList:pop()
    if self._size >0 then
        return self:removeAt(self._size);
    end
        return nil
end

function ArrayList:totable()
    local tab = {}
    for i = 1, self:size() do
        table.insert(tab, self._data[i])
    end
    return tab
end

--- @returns {int} size of the list
function ArrayList:size()
    return self._size
end

--- Adds items to the list.
-- @arg {vararg[any]} vararg - values to add to the list
-- @returns {List} modified list
function ArrayList:addMany(...)
    local args = table.pack(...)
    for k, v in ipairs(args) do
        self:add(v)
    end
    return self
end


function ArrayList:addAll(another)
    for i = 1, another._size do
        self._data[self._size + i] = another._data[i]
    end
    self._size = self._size + another._size
end

--- Returns whether the list contains a value.
-- @arg {any} val - value to check.
-- @returns {boolean} whether `val` is in the list
function ArrayList:contains(val)
    for i = 1, self:size() do
        if self:get(i) == val then
            return true
        end
    end
    return false
end

--- @returns {List} a copy of this list
function ArrayList:copy()
    local arr = self:new()
    for i = 1, self:size() do
        arr:add(self:get(i))
    end
    return arr
end

--- @returns {boolean} whether the list is empty
function ArrayList:isEmpty()
    return self._size == 0
end

function ArrayList:clear()
    if (self._size > 0) then
        for i = 1, self._size do
            self._data[i] = nil
        end
        self._size = 0;
    end
end

--- Returns a copy of a segment of this list.
-- 
-- @arg {int} start - start of the segment
-- @arg {int=end} finish - start of the segment
function ArrayList:sublist(start, finish)
    finish = finish or self:size()
    local sub = self.new()
    self:assertValidIndex(start)
    self:assertValidIndex(finish)
    for i = start, finish do sub:add(self:get(i)) end
    return sub
end

--- Sorts the list in place.
--  @arg {int=1} start - start index of the sort
--  @arg {int=end} finish - end index of the sort
function ArrayList:sort(start, finish)
    local partition = function (l, start, finish)
        local pivotIndex = math.random(start, finish)
        local pivot = self:get(pivotIndex)
        self:swap(pivotIndex, finish)
        local write = start
        for i = start, finish-1 do
            if self:get(i) < pivot then
                self:swap(i, write)
                write = write + 1
            end
        end
        self:swap(write, finish)
        return write
    end
    start = start or 1
    finish = finish or self:size()
    if start < finish then
        local pivot = partition(self, start, finish)
        self:sort(start, pivot-1)
        self:sort(pivot+1, finish)
    end
end

---自定义方法排序数组
---优先级从高到低排序，函数返回true或者1
---优先级从低到高排序，函数返回false或-1
function ArrayList:customSort(compareFunc, obj, start, finish)
    local partition = function (l, start, finish)
        local pivotIndex = math.random(start, finish)
        local pivot = self:get(pivotIndex)
        self:swap(pivotIndex, finish)
        local write = start
        for i = start, finish-1 do
            local result = compareFunc(obj, self:get(i), pivot)
            if (type(result) == "boolean" and not result) or (type(result) == "number" and result < 0) then
                self:swap(i, write)
                write = write + 1
            end
        end
        self:swap(write, finish)
        return write
    end
    start = start or 1
    finish = finish or self:size()
    if start < finish then
        local pivot = partition(self, start, finish)
        self:customSort(compareFunc, obj, start, pivot-1)
        self:customSort(compareFunc, obj, pivot+1, finish)
    end
end

function ArrayList:getMax(compareFunc, obj, start, finish)
    start = start or 1
    finish = finish or self:size()
    local max = self:get(start)
    if compareFunc == nil then
        compareFunc = function(l, i, j) 
            return i < j
        end
    end
    if start < finish then
        for i = start + 1, finish do
            local result = compareFunc(obj, max, self:get(i))
            if result then
                max = self:get(i)
            end
        end
    end
    return max
end

--- Asserts that index is inside the list.
-- @arg {int} index - index to check
function ArrayList:assertValidIndex(index)
    if index < 1 or index > self._size then
        error('index  is out of bounds for array of size ');
    end
end

--- @returns {string} string representation
function ArrayList:__tostring__()
    local s = '['
    for i = 1, self:size() do
        s = s .. tostring(self:get(i))
        if i ~= self:size() then
            s = s .. ', '
        end
    end
    s = s .. ']'
    return s
end

function ArrayList._next(t, index)
    return next(t._data, index)
end

function ArrayList.__pairs(t)
    return t._next, t, nil
end

return ArrayList
