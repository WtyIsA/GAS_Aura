require("HashSet")
require("ArrayList")

---@class HashMap
HashMap = {}
HashMap.__index = HashMap

---@return HashMap
function HashMap:new(key_values)
    local o = {}
    o._data = key_values or {}
    o._size = 0
    setmetatable(o, HashMap)
    if (key_values) then
        local s = 0
        for k, v in pairs(key_values) do
            s = s + 1
        end
        o._size = s
    end
    return o
end

function HashMap:add(key, val)
    local existOld = (self._data[key] ~= nil);
    self._data[key] = val
    if not existOld then
        self._size = self._size + 1
    end
    return self
end

function HashMap:addMany(tab)
    for k, v in pairs(tab) do
        self:add(k, v)
    end
    return self
end

function HashMap:size()
    return self._size
end

function HashMap:isEmpty()
    return self._size == 0
end

function HashMap:clear()
    if (self._size > 0) then
        for k, v in pairs(self._data) do
            self._data[k] = nil
        end
        self._size = 0
    end
end

function HashMap:copy()
    return HashMap.new():addMany(self:totable())
end

function HashMap:contains(key)
    return self._data[key] ~= nil
end

function HashMap:containsValue(value)
    for _, v in pairs(self._data) do
        if(v == value) then
            return true
        end
    end
    return false
end

function HashMap:get(key, returnNilIfMissing)
    if self:contains(key) then
        return self._data[key]
    else
        return nil
    end
end

function HashMap:remove(key)
    if (self:contains(key)) then
        local val = self:get(key)
        self._data[key] = nil
        self._size = self._size - 1
        return val
    end
    return nil
end

function HashMap:keys()
    local keys = HashSet:new()
    for k, v in pairs(self._data) do
        keys:add(k)
    end
    return keys
end

function HashMap:keysToList()
    local keys = ArrayList:new()
    for k, v in pairs(self._data) do
        keys:add(k)
    end
    return keys
end

function HashMap:values()
    local values = ArrayList:new()
    for k, v in pairs(self._data) do
        values:add(v)
    end
    return values
end

function HashMap:totable()
    local tab = {}
    for k, v in pairs(self._data) do
        tab[k] = v
    end
    return tab
end

function HashMap:__tostring__()
    local s = '{'
    local keys = self:keys():totable()

    for i = 1, self:size() do
        local key = keys[i]
        s = s .. tostring(key) .. ' -> ' .. tostring(self:get(key))
        if i ~= self:size() then
            s = s .. ', '
        end
    end
    s = s .. '}'
    return s
end

function HashMap:equals(another)
    if self:size() ~= another:size() then return false end
    for k, v in pairs(self._data) do
        if v ~= another._data[k] then return false end
    end
    return true
end

function HashMap._next(t, index)
    return next(t._data, index)
end

function HashMap.__pairs(t)
    return t._next, t, nil
end

return HashMap