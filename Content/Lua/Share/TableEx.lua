
TableEx = {}

function TableEx:GetTableLen(pTable)
    if(pTable == nil)then
        return 0;
    end
    local nCount = 0;
    for k,v in pairs(pTable) do
        nCount = nCount + 1;
    end
    return nCount;
end

function TableEx:ReverserTable(pTable)
    if(pTable == nil)then
        return 0;
    end
    local nCount = 0;
    for k,v in pairs(pTable) do
        print("key=",k, ",value=",v)
    end
    return nCount;
end

return TableEx;

