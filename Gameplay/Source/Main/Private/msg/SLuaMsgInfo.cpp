#include "SLuaMsgInfo.h"
#include <string.h>
#include "LuaManger.h"
#include "LuaMessageTableOptInC.h"
#include "UnLuaBase.h"
#include "Net/SessionMgr.h"

void SLuaMsgInfo::AnalysisMsg(const FString& formatInfo, TArray<int>& array)
{
    TArray<FString> arrayTypes;
    formatInfo.ParseIntoArray(arrayTypes,TEXT(","),false);
    for (auto strType : arrayTypes)
    {
	    array.Add(FCString::Atoi(*strType));
    }
}

int SLuaMsgInfo::GetMsgLen()
{
    if(m_nType == LUA_MSG_MSG)
        return m_nVariableCount + 2;
    return m_nVariableCount;
}


bool SLuaMsgInfo::MarshaBaseType(int key, FMarshal* obs, int KeyType)
{
    if(obs == nullptr)
        return false;
    switch (KeyType)
    {        
    case LUA_MSG_TYPE_BYTE:
        {
            int8 nTemp = key;
            obs->Write(nTemp);
        }
        break;
    case LUA_MSG_TYPE_SHORT:
        {
            int16 nTemp = key;
            obs->Write(nTemp);
        }
        break;
    case LUA_MSG_TYPE_INT:
        {
            int32 nTemp = key;
            obs->Write(nTemp);
        }
        break;
    case LUA_MSG_TYPE_LONG:
        {
            int64 nTemp = key;
            obs->Write(nTemp);
        }
        break;
    case LUA_MSG_TYPE_FLOAT:
        {
            float fTemp = key;
            obs->Write(fTemp);
        }
        break;
    case LUA_MSG_TYPE_DOUBLE:
        {
            double fTemp = key;
            obs->Write(fTemp);
        }
        break;
    default:
        return false;
    }
    return  true;
}

template <typename T>
bool SLuaMsgInfo::MarshalBaseType(T a, FMarshal* obs)
{
    if(obs == nullptr)
        return false;
    obs->Write(a);
    return true;
}


bool SLuaMsgInfo::MarshaBaseType(TValue* ptvalue, FMarshal* obs, int ValueType)
{
    if(ptvalue == nullptr || obs == nullptr)
        return false;
    switch (ValueType)
    {
    case LUA_MSG_TYPE_BOOL:
        {
            if(!ttisboolean(ptvalue))
                return false;
            bool bTemp = false;
            bTemp = CLuaMessageTableOptInC::GetBoolValue(ptvalue);
            obs->Write(bTemp);
        }
        break;
    case LUA_MSG_TYPE_BYTE:
        {
            if(!ttisinteger(ptvalue))
                return false;
            int8 nTemp = 0;
            nTemp = ivalue(ptvalue);
            obs->Write(nTemp);
        }
        break;
    case LUA_MSG_TYPE_SHORT:
        {
            if(!ttisinteger(ptvalue))
                return false;
            int16 nTemp = 0;
            nTemp = ivalue(ptvalue);
            obs->Write(nTemp);
        }
        break;
    case LUA_MSG_TYPE_INT:
        {
            if(!ttisinteger(ptvalue))
                return false;
            int32 nTemp = ivalue(ptvalue);
            obs->Write(nTemp);
        }
        break;
    case LUA_MSG_TYPE_LONG:
        {
            if(!ttisinteger(ptvalue))
                return false;
            int64 nTemp  = ivalue(ptvalue);
            obs->Write(nTemp);
        }
        break;
    case LUA_MSG_TYPE_FLOAT:
        {
            float fTemp = 0;
            if(ttisfloat(ptvalue))
                fTemp = fltvalue(ptvalue);
            else if(ttisinteger(ptvalue))
                fTemp = ivalue(ptvalue);
            else
                return false;
            obs->Write(fTemp);
        }
        break;
    case LUA_MSG_TYPE_DOUBLE:
        {
            double fTemp = 0;
            if(ttisfloat(ptvalue))
                fTemp = fltvalue(ptvalue);
            else if(ttisinteger(ptvalue))
                fTemp = ivalue(ptvalue);
            else
                return false;
            obs->Write(fTemp);
        }
        break;
    case LUA_MSG_TYPE_STRING:
        {
            if(!ttisstring(ptvalue))
                return false;
            std::string str(svalue(ptvalue));
            obs->WriteStr(str);
        }
        break;
    default:
        UE_LOG(GAME,Error,TEXT("[msg]MarshaBaseType,error,type(%d)"), ValueType);
        return false;
    }
    return  true;
}



bool SLuaMsgInfo::Marshal(struct Table* table, FMarshal* obs)
{
    return MarshalArray(table, obs, m_subTypes);
}

bool SLuaMsgInfo::MarshalArray(Table* table, FMarshal* obs, const TArray<int>& array, int tabStartPos)
{    
    int nMsgPos = 0;
    unsigned int nArrayPos = 0;
    if(m_nType == LUA_MSG_MSG)
    {
        nArrayPos += 2;
    }
    else if(m_nType == LUA_MSG_RPC)
    {
        nArrayPos += 4 + tabStartPos;
    }
    const int nSubTypeNum =  array.Num();
    for(; nMsgPos < nSubTypeNum;)
    {
        if(nArrayPos >= table->alimit)
            return false;
        TValue* pTempValue = &table->array[nArrayPos];
        int nValueType = array[nMsgPos];
        if(nValueType < LUA_MSG_TYPE_LIST)
        {
            if(!MarshaBaseType(pTempValue, obs, nValueType))
                return false;
        }
        else if(nValueType == LUA_MSG_TYPE_LIST)
        {
            if(!ttistable(pTempValue))
                return false;
            Table* pTempTable = hvalue(pTempValue);
            if(!MarshalList(pTempTable, obs, array[nMsgPos + 1]))
                return false;
            nMsgPos++;
        }
        else if(nValueType == LUA_MSG_TYPE_SET)
        {
            if(!ttistable(pTempValue))
                return false;
            Table* pTempTable = hvalue(pTempValue);
            if(!MarshalSet(pTempTable, obs, array[nMsgPos + 1]))
                return false;
            nMsgPos++;
        }
        else if(nValueType == LUA_MSG_TYPE_MAP)
        {
            if(!ttistable(pTempValue))
                return false;
            Table* pTempTable = hvalue(pTempValue);
            if(!MarshalMap(pTempTable, obs, array[nMsgPos + 1], array[nMsgPos + 2]))
                return false;
            nMsgPos += 2;
        }
        else if(nValueType == LUA_MSG_TYPE_OCTETS)
        {
            if(!ttistable(pTempValue))
                return false;
            Table* pTempTable = hvalue(pTempValue);
        }
        else  
        {
            if(!ttistable(pTempValue))
                return false;
            Table* pTempTable = hvalue(pTempValue);
            if(!m_pSessionMgr->Marshal(pTempTable, obs, nValueType))
                return false;
        }
        
        nMsgPos++;
        nArrayPos++;
    }
    return  true;
}

bool SLuaMsgInfo::MarshalSet(Table* table, FMarshal* obs, int KeyType)
{
    if(table == nullptr || obs == nullptr)
        return false;
    int nMapSize = CLuaMessageTableOptInC::GetNumber(table, "_size");
    table = CLuaMessageTableOptInC::GetTable(table, "_data");
    int nCount = table->alimit;
    Node* pNode;    
    obs->Write(nMapSize);

    for(int i = 1; i <= nCount; i++)
    {
        TValue* pTempValue = &table->array[i-1];
        if(ttisnil(pTempValue))
            continue;
        if(KeyType < LUA_MSG_TYPE_LIST)
        {
            if(!MarshaBaseType(i, obs, KeyType))
                return false;
        }
    }
    
    nCount = 1 << table->lsizenode;
    for(int i = 0; i < nCount; i++)
    {
        pNode = &table->node[i];
        if(pNode == nullptr)
            continue;
        
        if(&pNode->u.key_tt == LUA_TNIL || ttisnil(&pNode->i_val))
        {
            continue;
        }
        if(ttisfunction(&pNode->i_val))
        {            
            continue;
        }

        TValue pTempValue;
        CLuaMessageTableOptInC::GetKeyFromNode(pNode, pTempValue);
        if(KeyType < LUA_MSG_TYPE_LIST)
        {
            if(!MarshaBaseType(&pTempValue, obs, KeyType))
                return false;
        }
        else
        {
            if(!ttistable(&pTempValue))
                return false;
            Table* pTempTable = hvalue(&pTempValue);
            if(!m_pSessionMgr->Marshal(pTempTable, obs, KeyType))
                return false;
        }
    }
    return true;
}

//Map: key只能是基本类型
bool SLuaMsgInfo::MarshalMap(struct Table* table, FMarshal* obs, int KeyType, int ValueType)
{
    if(table == nullptr || obs == nullptr)
        return false;
    int nMapSize = CLuaMessageTableOptInC::GetNumber(table, "_size");
    table = CLuaMessageTableOptInC::GetTable(table, "_data");
    int nCount = table->alimit;
    Node* pNode;
    obs->Write(nMapSize);

    for(int i = 1; i <= nCount; i++)
    {
        TValue* pTempValue = &table->array[i-1];
        if(ttisnil(pTempValue))
            continue;
        if(KeyType < LUA_MSG_TYPE_STRING)
        {
            if(!MarshaBaseType(i, obs, KeyType))
                return false;
        }
        else
        {
            UE_LOG(GAME, Error, TEXT("[msg]MarshalMap array part key count be bin:%d"), KeyType);
            continue;
        }        
        
        if(ValueType < LUA_MSG_TYPE_LIST)
        {
            if(!MarshaBaseType(pTempValue, obs, ValueType))
                return false;
        }
        else
        {
            if(!ttistable(pTempValue))
                return false;
            Table* pTempTable = hvalue(pTempValue);
            if(!m_pSessionMgr->Marshal(pTempTable, obs, ValueType))
                return false;
            
        }
    }
    
    nCount = 1 << table->lsizenode;
    for(int i = 0; i < nCount; i++)
    {
        pNode = &table->node[i];
        if(pNode == nullptr)
            continue;
        if(pNode->u.key_tt == LUA_TNIL || ttisnil(&pNode->i_val))
        {
            continue;
        }
        if(ttisfunction(&pNode->i_val))
        {            
            continue;
        }

        TValue pTempValue;
        CLuaMessageTableOptInC::GetKeyFromNode(pNode, pTempValue);
        if(KeyType < LUA_MSG_TYPE_LIST)
        {
            if(!MarshaBaseType(&pTempValue, obs, KeyType))
                return false;
        }
        else
        {
            UE_LOG(GAME, Error, TEXT("[msg]MarshalMap key count be bin:%d"), KeyType);
            continue;;
        }
        
        if(ValueType < LUA_MSG_TYPE_LIST)
        {
            if(!MarshaBaseType(&pNode->i_val, obs, ValueType))
                return false;
        }
        else
        {
            if(!ttistable(&pNode->i_val))
                return false;
            Table* pTempTable = hvalue(&pNode->i_val);
            if(!m_pSessionMgr->Marshal(pTempTable, obs, ValueType))
                return false;
        }
    }
    return true;
}

//List 不能直接套List和Map,比如不能List<List<Info>>,需要改成List<TempInfo>其中 TemInfo=List<Info>
bool SLuaMsgInfo::MarshalList(struct Table* table, FMarshal* obs, int ValueType)
{
    if(table == nullptr || obs == nullptr)
        return false;
    int nCount = CLuaMessageTableOptInC::GetNumber(table,"_size");
    table = CLuaMessageTableOptInC::GetTable(table, "_data");
    
    obs->Write(nCount);
    for(int i = 0; i < nCount; i++)
    {
        TValue* pTempValue = &table->array[i];
        if(ttisnil(pTempValue))
            continue;
        if(ValueType < LUA_MSG_TYPE_LIST)
        {
            if(!MarshaBaseType(pTempValue, obs, ValueType))
                return false;
        }
        else
        {
            if(!ttistable(pTempValue))
                return false;
            Table* pTempTable = hvalue(pTempValue);
            if(!m_pSessionMgr->Marshal(pTempTable, obs, ValueType))
                return false;
            
        }
    }
    
    return  true;
}




bool SLuaMsgInfo::UnMarshal(FUnMarshal* ibs, Table* table)
{
    return UnMarshalArray(ibs,table,m_subTypes);
}

bool SLuaMsgInfo::UnMarshalArray(FUnMarshal* ibs, Table* table,const TArray<int>& array, int tabStartPos)
{
    if(table == nullptr || ibs == nullptr)
        return false;    
    int nMsgPos = 0;
    int nArrayPos = 1;
    if(m_nType == LUA_MSG_MSG)
    {
        nArrayPos += 2;        
    }
    else if(m_nType == LUA_MSG_RPC)
    {
        nArrayPos += 4 + tabStartPos;
    }
    const int nSubTypeNum =  array.Num();
    for(; nMsgPos < nSubTypeNum;)
    {
        int nValueType = array[nMsgPos];
        if(nValueType < LUA_MSG_TYPE_LIST)
        {
            TValue uValue ;
            if(!UnMarshalTValue(ibs, nValueType, uValue))
                return false;
            CLuaMessageTableOptInC::AddTValue(table,  nArrayPos, &uValue);
        }
        else if(nValueType == LUA_MSG_TYPE_LIST)
        {
            //List : {{},{},{}} 
            int nListValue = array[nMsgPos+1];
            bool bUnmarshalResult = true;
            int nArraySize = ibs->Read<int32>(bUnmarshalResult);
            if(!bUnmarshalResult)
                return false;
            Table* pSubTabel = m_pSessionMgr->CreateTable(nValueType, 1,2); //List对应ArrayList
            CLuaMessageTableOptInC::AddTable(table, nArrayPos, pSubTabel);
            if(!UnMarshalList(ibs, pSubTabel,nArraySize, nListValue))
                return false;
            nMsgPos++;
        }
        else if(nValueType == LUA_MSG_TYPE_SET)
        {
            int nListValue = array[nMsgPos+1]; 
            Table* pSubTabel = m_pSessionMgr->CreateTable(nValueType, 1,2);
            CLuaMessageTableOptInC::AddTable(table, nArrayPos, pSubTabel);
            if(!UnMarshalSet(ibs, pSubTabel, nListValue))
                return false;
            nMsgPos++;
        }
        else if(nValueType == LUA_MSG_TYPE_MAP)
        {
            Table* pSubTabel = m_pSessionMgr->CreateTable(nValueType); //List对应LuaMap
            CLuaMessageTableOptInC::AddTable(table, nArrayPos, pSubTabel);
            if(!UnMarshalMap(ibs, pSubTabel, array[nMsgPos+1], array[nMsgPos+2]))
                return false;
            nMsgPos += 2;
        }
        else if(nValueType == LUA_MSG_TYPE_OCTETS)
        {
            ///TODO: OCTETS
        }
        else
        {
            Table* pSubTable = m_pSessionMgr->CreateTable(nValueType);
            CLuaMessageTableOptInC::AddTable(table, nArrayPos, pSubTable);
            if(!m_pSessionMgr->UnMarshal(0,ibs, pSubTable, nValueType))
                return false;
        }
        nMsgPos++;
        nArrayPos++;
    }
    return true;
}

//ArrayList : { _data={{},{},{}}, _size=num}
bool SLuaMsgInfo::UnMarshalList(FUnMarshal* ibs, Table* table, int nArraySize, int nValueType)
{
    if(table == nullptr || ibs == nullptr)
        return false;
    Table* pDataTable = CLuaMessageTableOptInC::CreateNewTable(nullptr, nArraySize, 1);
    CLuaMessageTableOptInC::AddTable(table, "_data", pDataTable);
    CLuaMessageTableOptInC::AddValue(table, "_size", nArraySize);
            
    for(int index = 1; index <= nArraySize; index++)
    {
        TValue uValue ;
        if(!UnMarshalTValue(ibs, nValueType, uValue))
            return false;
        CLuaMessageTableOptInC::AddTValue(pDataTable,  index, &uValue);        
    }
    return true;
}

bool SLuaMsgInfo::UnMarshalSet(FUnMarshal* ibs, Table* table, int KeyType)
{
    if(table == nullptr || ibs == nullptr)
        return false;
    lua_State *L = UnLua::GetState();
    bool bUnMarshalResult = true;
    int nMapCount = ibs->Read<int32>(bUnMarshalResult);
    if(!bUnMarshalResult)
        return false;
    Table* pDataTable = CLuaMessageTableOptInC::CreateNewTable(nullptr, nMapCount, 8);
    CLuaMessageTableOptInC::AddTable(table, "_data", pDataTable);
    CLuaMessageTableOptInC::AddValue(table, "_size", nMapCount);
    for(int index = 0; index < nMapCount; index++)
    {
        TValue key ;
        if(!UnMarshalTValue(ibs, KeyType,key))
            return false;
        TValue* pNewValue = luaH_set(L, pDataTable, &key);
        setobj(L,pNewValue, &key);
    }

    return  true;
}

bool SLuaMsgInfo::UnMarshalMap(FUnMarshal* ibs, Table* table, int KeyType, int ValueType)
{
    if(table == nullptr || ibs == nullptr)
        return false;
    lua_State *L = UnLua::GetState();
    bool bUnMarshalResult = true;
    int nMapCount = ibs->Read<int32>(bUnMarshalResult);
    if(!bUnMarshalResult)
        return false;
    Table* pDataTable = CLuaMessageTableOptInC::CreateNewTable(nullptr, nMapCount, 8);
    CLuaMessageTableOptInC::AddTable(table, "_data", pDataTable);
    CLuaMessageTableOptInC::AddValue(table, "_size", nMapCount);
    for(int index = 0; index < nMapCount; index++)
    {
        TValue key ;
        if(!UnMarshalTValue(ibs, KeyType, key))
            return false;
        TValue uValue;
        if(!UnMarshalTValue(ibs, ValueType, uValue))
            return false;
        TValue* pNewValue = luaH_set(L, pDataTable, &key);
        setobj(L,pNewValue, &uValue);
    }

    return  true;
}

bool SLuaMsgInfo::UnMarshalTValue(FUnMarshal* ibs, int nType, TValue& retValue)
{    
    if(ibs == nullptr)
    {
        return false;
    }
    bool bUnMarshalResult = true;
    switch (nType)
    {
    case LUA_MSG_TYPE_BYTE:
        {
            int8 n = ibs->Read<int8>(bUnMarshalResult);
            if(!bUnMarshalResult)
                return false;
            setivalue(&retValue, n);
        }
        break;
    case LUA_MSG_TYPE_BOOL:
        {
            bool b = ibs->Read<int8>(bUnMarshalResult) > 0;
            if(!bUnMarshalResult)
                return false;
            CLuaMessageTableOptInC::SetBoolValue(&retValue, b);
        }
        break;
    case LUA_MSG_TYPE_SHORT:
        {
            int16 n = ibs->Read<int16>(bUnMarshalResult);
            if(!bUnMarshalResult)
                return false;
            setivalue(&retValue, n);
        }
        break;
    case LUA_MSG_TYPE_INT:
        {
            int32 n = ibs->Read<int32>(bUnMarshalResult);
            if(!bUnMarshalResult)
                return false;
            setivalue(&retValue, n);
        }
        break;
    case LUA_MSG_TYPE_LONG:
        {
            int64 n = ibs->Read<int64>(bUnMarshalResult);
            if(!bUnMarshalResult)
                return false;
            setivalue(&retValue, n);
        }
        break;
    case LUA_MSG_TYPE_FLOAT:
        {
            float n = ibs->Read<float>(bUnMarshalResult);
            if(!bUnMarshalResult)
                return false;
            setfltvalue(&retValue, n);
        }
        break;
    case LUA_MSG_TYPE_DOUBLE:
        {
            double n = ibs->Read<double>(bUnMarshalResult);
            if(!bUnMarshalResult)
                return false;
            setfltvalue(&retValue, n);
        }
        break;
    case LUA_MSG_TYPE_STRING:
        {
            lua_State *L = UnLua::GetState();
            std::string str;
            ibs->ReadStr(str,bUnMarshalResult);
            if(!bUnMarshalResult)
                return false;
            TString* toLuaStr =  luaS_new(L, str.c_str());
            setsvalue(L, &retValue, toLuaStr);
        }
        break;
    default:
        {
            Table* pSubTable = m_pSessionMgr->CreateTable(nType);
            if(!m_pSessionMgr->UnMarshal(0,ibs, pSubTable, nType))
                return false;
            lua_State *L = UnLua::GetState();
            sethvalue(L, &retValue, pSubTable);
        }
        break;        
    }
    return  true;
}

template <typename T>
bool SLuaMsgInfo::UnMarshalBaseType(T& result, FUnMarshal* ibs)
{
    if(ibs == nullptr)
        return false;
    bool bUnMarshalResult = true;
    result = ibs->Read<T>(bUnMarshalResult);
    return bUnMarshalResult;
}

void SLuaMsgInfo::GetMsgVariableCount(const TArray<int>& array, uint8& nCount)
{
    int nMsgPos = 0;
    nCount = 0;
    const int nSubTypeNum =  array.Num();
    for(; nMsgPos < nSubTypeNum;)
    {
        int nValueType = array[nMsgPos];
        if(nValueType == LUA_MSG_TYPE_LIST)
        {
            nMsgPos++;
        }
        else if(nValueType == LUA_MSG_TYPE_SET)
        {            
            nMsgPos++;
        }
        else if(nValueType == LUA_MSG_TYPE_MAP)
        {
            nMsgPos += 2;
        }
        
        nMsgPos++;
        nCount++;
    }
}
