require("HashMap")
require("ArrayList")

---@class CachedCollections
CachedCollections = {}

local Prototype2Collections = HashMap:new()

---@param protoType HashMap | HashSet | ArrayList | LinkedList
---@param values  HashMap | HashSet | ArrayList | LinkedList | nil
---@return HashMap | HashSet | ArrayList | LinkedList
function CachedCollections:New(protoType, values)
    local collection
    local collectionsList = Prototype2Collections:get(protoType)
    if(collectionsList ~= nil and collectionsList:size() > 0) then
        collection = collectionsList:removeAt(collectionsList:size())
    else
        collection =  protoType:new()
    end
    collection:clear()
    if(values ~= nil) then
        CachedCollections.Copy(values,collection)
    end
    return collection
end

---@param collection HashMap | HashSet | ArrayList | LinkedList
function CachedCollections:Recycle(collection)
    if(collection == nil) then
        return
    end
    local protoType = getmetatable(collection)
    if(protoType ~= nil) then
        ---@type LinkedList
        local collectionsList = Prototype2Collections:get(protoType)
        if(collectionsList == nil) then
            collectionsList = ArrayList:new()
            Prototype2Collections:add(protoType,collectionsList)
        end
        collection:clear()
        collectionsList:add(collection)
    end
end

---@private
function CachedCollections.Copy(from,to)
    local mt = getmetatable(from)
    if(mt == HashMap)  then
        to:addMany(from)
    else
        for _,v in pairs(from) do
            to:add(v)
        end
    end
end