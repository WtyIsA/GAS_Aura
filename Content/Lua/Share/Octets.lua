
---@class Octets
local Octets = {}

Octets.__index = function(t,k) 
    local meta = getmetatable(t)
    if meta then
        return rawget(meta,k)
    end
    return nil
end

function Octets:new(strData)
    local o = {}
    o.data = strData or ""
    o.offset = 1
    setmetatable(o,self)
    return o
end

function Octets:Push_Int(iValue)
    self.data = self.data..string.pack("i",iValue)
end

function Octets:Push_Float(fValue)
    --precision problem
    local a,b = math.modf(fValue)
    self:Push_Int(#tostring(b)-2)
    self.data = self.data..string.pack("f",fValue)
end

function Octets:Push_Double(dValue)
    self.data = self.data..string.pack("d",dValue)
end

function Octets:Push_String(sValue)
    self.data = self.data..string.pack("s",sValue)
end

function Octets:Push_Byte(bValue)
    self.data = self.data..string.pack("b",bValue)
end

function Octets:Push_Bool(bValue)
    self:Push_Byte(bValue and 1 or 0)
end

function Octets:Pop_Int()
    local result
    result,self.offset = string.unpack("i",self.data,self.offset)
    return tonumber(result)
end

function Octets:Pop_Float()
    local rounding,result = self:Pop_Int()
    result,self.offset = string.unpack("f",self.data,self.offset)
    return tonumber(string.format("%."..tostring(rounding).."f",result))
end

function Octets:Pop_Double()
    local result
    result,self.offset = string.unpack("d",self.data,self.offset)
    return tonumber(result)
end

function Octets:Pop_String()
    local result
    result,self.offset = string.unpack("s",self.data,self.offset)
    return result
end

function Octets:Pop_Byte()
    local result
    result,self.offset = string.unpack("b",self.data,self.offset)
    return tonumber(result)
end

function Octets:Pop_Bool()
    return self:Pop_Byte() > 0
end

function Octets:ResetOffset()
    self.offset = 1
end

function Octets:GetContest()
    return self.data
end

function Octets:Clear()
    self.data = ""
    self.offset = 1
end

return Octets