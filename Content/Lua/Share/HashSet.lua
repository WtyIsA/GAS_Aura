
---@class HashSet
HashSet = {}
HashSet.__index = HashSet

--- Constructor.
-- @arg {table[any]=} values - used to initialize the HashSet
function HashSet:new(values)
    local o = {}
    values = values or {}
    o._data = values
    o._size = values
    setmetatable(o, HashSet)
    if (values) then
        local s = 0
        for k, v in pairs(values) do
            s = s + 1
        end
        o._size = s
    end
    return o
end

--- @arg {any} val - value to produce a key for
-- @returns {torch.pointer} unique key for the value
function HashSet.keyOf(val)
    
    return val
    
    --[[
    if type(val) == 'number' or type(val) == 'nil' or type(val) == 'string' then
        return val
    else
        return torch.pointer(val)
    end
    --]]
end

--- @returns {int} number of values in the HashSet
function HashSet:size()
    return self._size
end

--- @returns {boolean} whether the HashSet is empty
function HashSet:isEmpty()
    return self._size == 0
end

--- Adds a value to the HashSet.
-- @arg {any} val - value to add to the HashSet
-- @returns {HashSet} modified HashSet
function HashSet:add(val)
    local key = HashSet.keyOf(val)
    local existOld = (self._data[key] ~= nil)
    self._data[key] = val
    if not existOld then
        self._size = self._size + 1
    end
    return self
end

--- Adds a variable number of values to the HashSet.
-- @arg {vararg} vararg - values to add to the HashSet
-- @returns {HashSet} modified HashSet
function HashSet:addMany(...)
    local args = table.pack(...)
    for i, val in ipairs(args) do
        self:add(val)
    end
    return self
end

function HashSet:addAll(another)
    for _, val in pairs(another) do
        self:add(val)
    end
end

--- @returns {HashSet} copy of the HashSet
function HashSet:copy()
    return HashSet.new(self, self:totable())
end

--- @returns {boolean} whether the HashSet contains `val`
-- @arg {any} val - value to check for
function HashSet:contains(val)
    local key = HashSet.keyOf(val)
    return self._data[key] ~= nil
end

--- @arg {any} val - value to remove from the HashSet.
function HashSet:remove(val)
    if (self:contains(val) == true) then
        local key = HashSet.keyOf(val)
        self._data[key] = nil
        self._size = self._size - 1
        return true
    end
    return false
end

function HashSet:clear()
    if (self._size > 0) then
        for k, v in pairs(self._data) do
            self._data[k] = nil
        end
        self._size = 0
    end
end

--- @returns {tabl} the HashSet in table format
function HashSet:totable()
    local tab = {}
    for k, v in pairs(self._data) do
        table.insert(tab, v)
    end
    return tab
end

--- Compares two HashSets.
-- @arg {HashSet} another - another HashSet
-- @returns {boolean} whether this HashSet and `another` contain the same values
function HashSet:equals(another)
    if self:size() ~= another:size() then
        return false
    end

    for i, v in ipairs(another:totable()) do
        if not self:contains(v) then
            return false
        end
    end
    return true
end

--- Computes the union of two HashSets.
-- @arg {HashSet} another - another HashSet
-- @returns {HashSet} a HashSet of values that are in this HashSet or in `another`
function HashSet:union(another)
    local s = self:copy()
    for i, v in ipairs(another:totable()) do
        s:add(v)
    end
    return s
end

--- Computes the intersection of two HashSets.
-- @arg {HashSet} another - another HashSet
-- @returns {HashSet} a HashSet of values that are in this HashSet and in `another`
function HashSet:intersect(another)
    local s = self:copy()
    for i, v in ipairs(self:totable()) do
        if not another:contains(v) then
            s:remove(v)
        end
    end
    return s
end

--- Subtracts another HashSet from this one.
-- @arg {HashSet} another - another HashSet
-- @returns {HashSet} a HashSet of values that are in this HashSet but not in `another`
function HashSet:subtract(another)
    local s = self:copy()
    for i, v in ipairs(self:totable()) do
        if another:contains(v) then
            s:remove(v)
        end
    end
    return s
end

--- @returns {string} string representation
function HashSet:__tostring__()
    local s = '('
    local keys = self:totable()
    for i = 1, self:size() do
        key = keys[i]
        s = s .. tostring(key)
        if i ~= self:size() then
            s = s .. ', '
        end
    end
    s = s .. ')'
    return s
end

function HashSet._next(t, index)
    return next(t._data, index)
end

function HashSet.__pairs(t)
    return t._next, t, nil
end

return HashSet