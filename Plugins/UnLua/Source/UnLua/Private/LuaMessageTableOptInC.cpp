#include "LuaMessageTableOptInC.h"

#include "UnLua/Public/UnLuaEx.h"
#include "UnLua/Public/UnLuaBase.h"
#include "lstate.h"
#include "ltable.h"
#include "lstring.h"
#include "lualib.h"
#include "lapi.h"
//#include "../../../../Lua/src/lapi.h"
//using namespace UnLua;

#define CheckIntGeNumber(o, n)  if((o) < n) (o)=n;
#define CheckTableArrayShouldResize(t, n, l) if( t->alimit < (unsigned int)(n)) IncreaseTableArrays(t,(n)*2, l);



CLuaMessageTableOptInC* CLuaMessageTableOptInC::Get()
{
    static CLuaMessageTableOptInC handle;
    return  &handle;
}

CLuaMessageTableOptInC::CLuaMessageTableOptInC()
{
}

void CLuaMessageTableOptInC::IncreaseTableArrays(Table* pTable,int nNewArraySize, lua_State *Nl)
{
    if((unsigned int)nNewArraySize > pTable->alimit)
    {
        lua_State *L = Nl?Nl: UnLua::GetState();
        luaH_resizearray(L, pTable, nNewArraySize);
    }
}

Udata* CLuaMessageTableOptInC::CreateUsedata(int len,  int nuvalue, void* pMem,  lua_State *Nl)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
#if 504 == LUA_VERSION_NUM
    uint8* Dest = (uint8*)lua_newuserdatauv(L, sizeof(void*),nuvalue);    
    void** p = (void**)Dest;
    *p = pMem;
    Udata* ud = uvalue(s2v(L->top - 1));
#else
    uint8 *Dest = lua_newuserdata(L, sizeof(void*));
    void **p = (void**)Dest;
    *p = pMem;    
    Udata* ud = uvalue(s2v(L->top - 1));
#endif
    return ud;
}


Table* CLuaMessageTableOptInC::CreateNewTable(const char* metatablename,int nArraySize, int nHashSize, lua_State *Nl, bool bPushToTop)
{
    CheckIntGeNumber(nArraySize,1);
    CheckIntGeNumber(nHashSize,1);
    lua_State *L = Nl?Nl: UnLua::GetState();
    lua_createtable(L, nArraySize, nHashSize);    
    Table* p = (Table*)lua_topointer(L, -1);
   if(metatablename != nullptr)
    {
        int32 Type = lua_getglobal(L,metatablename);
        lua_setmetatable(L, -2);
        //不需要从栈顶移除， lua_setmetatable会自动从栈顶移除
    }
    if(!bPushToTop)
        lua_pop(L, 1);//从栈顶移除，否则有栈溢出而崩溃风险
    return  p;
}

bool CLuaMessageTableOptInC::SetTableMetatable(Table* pTable,const char* metatablename, lua_State *Nl)
{
    if(metatablename == nullptr || pTable == nullptr)
        return false;
    lua_State *L = Nl?Nl: UnLua::GetState();
    PushTable(pTable, L);
    int32 Type = lua_getglobal(L,metatablename);
    if(Type == LUA_TTABLE)
    {
        lua_setmetatable(L, -2);
        lua_pop(L,1);
        return true;
    }
    lua_pop(L,2);
    return false;
}

const TValue* CLuaMessageTableOptInC::GetTValue(const Table* table,FString& strKey, lua_State *Nl)
{
    return GetTValue(table, TCHAR_TO_UTF8(*strKey), Nl);
}

const TValue* CLuaMessageTableOptInC::GetTValue(const Table* table,const char* strKey, lua_State *Nl)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
    TString* toLuaStr =  luaS_new(L, strKey);    
    const TValue * pRet =  luaH_getstr((Table*)table, toLuaStr);
    if(pRet != nullptr && !ttisnil(pRet))
        return pRet;
    return nullptr;
}

const TValue* CLuaMessageTableOptInC::GetTValue(const Table* table,int64 nKey, lua_State *Nl)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
    const TValue * pRet =  luaH_getint((Table*)table, nKey);
    if(pRet != nullptr && !ttisnil(pRet))
        return pRet;
    return nullptr;
}

void CLuaMessageTableOptInC::GetNumber(const Table* table,const char* strKey, int&nValue, lua_State *Nl)
{
    const TValue* ptvalue = GetTValue(table, strKey, Nl);
    if(ptvalue != nullptr)
        nValue = ivalue(ptvalue);
}

void CLuaMessageTableOptInC::GetNumber(const Table* table,const char* strKey, uint8 &nValue, lua_State *Nl)
{
    const TValue* ptvalue = GetTValue(table, strKey, Nl);
    if(ptvalue != nullptr)
        nValue = ivalue(ptvalue);
}

int CLuaMessageTableOptInC::GetNumber(const Table* table,const char* strKey, lua_State *Nl)
{
    const TValue* ptvalue = GetTValue(table, strKey, Nl);
    int32 nTemp = 0;
    if(ptvalue != nullptr)
        nTemp = ivalue(ptvalue);
    return  nTemp;
}

Table* CLuaMessageTableOptInC::GetTable(const Table* table,const char* strKey, lua_State *Nl )
{
    const TValue* ptvalue = GetTValue(table, strKey, Nl);
    if(ptvalue == nullptr || !ttistable(ptvalue))
        return nullptr;
    return  hvalue(ptvalue);
}
TValue* CLuaMessageTableOptInC::InsertToTableGetValue(Table* table,const char*strKey, lua_State *Nl)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
    TValue key;
    TString* toLuaStr =  luaS_new(L, strKey);
    setsvalue(L, &key, toLuaStr);
    TValue* pValue = luaH_set(L,table, &key);
    return pValue;
}

TValue* CLuaMessageTableOptInC::InsertToTableGetValue(Table* table, lua_Integer nKey, lua_State *Nl)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
    TValue key;
    setivalue(&key, nKey);
    TValue* pValue = luaH_set(L,table, &key);
    return pValue;
}


void CLuaMessageTableOptInC::AddTable(Table* table,const char* strKey, Table* subTable, lua_State *Nl)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
    TValue* pTValue = InsertToTableGetValue(table,strKey, L);
    sethvalue(L, pTValue, subTable);
    
}
void CLuaMessageTableOptInC::AddTable(Table* table, int nIndex, Table* subTable, lua_State *Nl)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
    CheckTableArrayShouldResize(table, nIndex,L)
    TValue toAddValue;
    sethvalue(L, &toAddValue, subTable);
    luaH_setint(L, table, nIndex, &toAddValue);
}


void CLuaMessageTableOptInC::PushTable(Table* table, lua_State *Nl)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
    lua_lock(L);
    sethvalue(L, &L->top->val, table);
    api_incr_top(L);    
    luaC_checkGC(L);
    lua_unlock(L);
}

void CLuaMessageTableOptInC::PushValue(TValue* pValue, lua_State *Nl)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
    lua_lock(L);
    setobj(L, &L->top->val, pValue);
    api_incr_top(L);
    luaC_checkGC(L);
    lua_unlock(L);
}

void CLuaMessageTableOptInC::PushUserdata(Udata* ud, lua_State *Nl)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
    lua_lock(L);
    setuvalue(L, &L->top->val, ud);
    api_incr_top(L);
    luaC_checkGC(L);
    lua_unlock(L);
}

void CLuaMessageTableOptInC::PushLuaFunction(const void* pFun,lua_State *Nl)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
    lua_lock(L);
    setclLvalue(L, &L->top->val, (LClosure*)pFun);
    api_incr_top(L);
    luaC_checkGC(L);
    lua_unlock(L);
}

void CLuaMessageTableOptInC::AddTValue(Table* table, int nIndex, TValue* pV, lua_State *Nl)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
    CheckTableArrayShouldResize(table, nIndex, L);
    luaH_setint(L, table, nIndex, pV);
}

void CLuaMessageTableOptInC::AddTValueByKey(Table* table,unsigned int nKey, TValue* pV, lua_State* Nl)
{    
    lua_State *L = Nl?Nl: UnLua::GetState();
    luaH_setint(L, table, nKey, pV);
}


void CLuaMessageTableOptInC::AddValue(Table* table,const char* strKey, int n, lua_State *Nl)
{
    TValue* pValue = InsertToTableGetValue(table,strKey, Nl);
    setivalue(pValue, n);
}

void CLuaMessageTableOptInC::AddValue(Table* table, TValue* pKey, TValue*  pV, lua_State *Nl)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
    TValue* pValue = luaH_set(L,table, pKey);
    setobj2t(L, pValue, pV);
}

void CLuaMessageTableOptInC::AddNode(Table* table,Node* pNode, lua_State *Nl)
{
    if(pNode == nullptr)
        return;
    lua_State *L = Nl?Nl: UnLua::GetState();
#if 504 == LUA_VERSION_NUM
    TValue tempValue;
    getnodekey(L,&tempValue, pNode);
    AddValue(table, &tempValue, &pNode->i_val,L);
#else
    AddValue(table, &pNode->i_key.tvk, &pNode->i_val)
#endif

}

void CLuaMessageTableOptInC::AddThread(Table* table,lua_State *thread,lua_State *L)
{
    if(table == nullptr || thread == nullptr || L == nullptr)
        return;
    int nKey = luaH_getn(table) + 1;
    TValue toAddValue;
    setthvalue(L, &toAddValue, thread);
    luaH_setint(L, table, nKey, &toAddValue);
}

bool CLuaMessageTableOptInC::RemoveValue(Table* table,const char* strKey, lua_State *Nl)
{
    if(table == nullptr || strKey == nullptr || strlen(strKey) == 0)
        return false;
    lua_State *L = Nl?Nl: UnLua::GetState();
    TValue key;
    TString* toLuaStr =  luaS_new(L, strKey);
    setsvalue(L, &key, toLuaStr);
    return RemoveValue(table, &key, L);
}
bool CLuaMessageTableOptInC::RemoveValue(Table* table,int key, lua_State *Nl)
{
    if(table == nullptr)
        return false;
    TValue * pValue = (TValue * )luaH_getint(table, key);
    if(pValue == nullptr)
        return false;
    setnilvalue(pValue);
    return true;
}
bool CLuaMessageTableOptInC::RemoveValue(Table* table,TValue* pKey, lua_State *Nl)
{
    if(table == nullptr || pKey == nullptr)
        return false;
    TValue * pValue = (TValue * )luaH_get(table, pKey);
    if(pValue == nullptr)
        return false;
    setnilvalue(pValue);
    
    return true;
}

lua_State* CLuaMessageTableOptInC::CreateState()
{
    lua_State *L = lua_newthread(UnLua::GetState());
    return L;
}

void CLuaMessageTableOptInC::FreeState(lua_State* L)
{
    if(L == nullptr)
        return;
    luaE_freethread(UnLua::GetState(),L);
}

int CLuaMessageTableOptInC::PrintLuaTrackbackAndPushNil(const char* errorMsg, lua_State *Nl, int level)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
    luaL_traceback(L,L,errorMsg, level);
    const char* pLog = lua_tostring(L,-1);
    UE_LOG(LogCore,Error,TEXT("%s"), UTF8_TO_TCHAR(pLog));
    lua_pop(L,1);
    lua_pushnil(L);
    return  1;
}

int CLuaMessageTableOptInC::OnlyPrintLuaTrackback(const char* errorMsg, lua_State *Nl, int level)
{
    lua_State *L = Nl?Nl: UnLua::GetState();
    luaL_traceback(L,L,errorMsg, level);
    const char* pLog = lua_tostring(L,-1);
    UE_LOG(LogCore,Error,TEXT("%s"), UTF8_TO_TCHAR(pLog));
    lua_pop(L,1);
    return  0;
}

const char* CLuaMessageTableOptInC::GetTValueStr(const TValue* pValue)
{
    if(pValue == nullptr || !ttisstring(pValue))
        return nullptr;
    return svalue(pValue);
}

const char* CLuaMessageTableOptInC::GetTValueStr(const Value* pNode)
{
    return getstr(gco2ts(pNode->gc));
}

const char* CLuaMessageTableOptInC::GetTValueStr(const Table* table,const char* strKey, lua_State *Nl)
{
    if(table == nullptr || strKey == nullptr)
        return nullptr;
    const TValue* pValue = GetTValue(table, strKey, Nl);
    if(pValue == nullptr)
        return nullptr;
    return GetTValueStr(pValue);
}

bool CLuaMessageTableOptInC::SetValue(Table* table,const char* strKey, lua_Integer n, bool bAddIfNotExist, lua_State *Nl)
{
    if(table == nullptr || strKey == nullptr)
        return false;
    TValue * pValue = (TValue *)GetTValue(table, strKey,Nl);
    if(!pValue)
    {
        if(bAddIfNotExist)
            pValue = InsertToTableGetValue(table, strKey, Nl);
    }
    if( pValue)
    {
        setivalue(pValue, n);
        return true;
    }
    return false;
}

void CLuaMessageTableOptInC::SetBoolValue(TValue* pV, bool var)
{
    if(pV == nullptr)
        return;
#if 504 == LUA_VERSION_NUM
    if(var)
        setbtvalue(pV);
    else
        setbfvalue(pV);
#else
    setbvalue(pV, var);
#endif

}

bool CLuaMessageTableOptInC::GetBoolValue(const TValue* pV)
{
    if(pV == nullptr)
        return  false;
#if 504 == LUA_VERSION_NUM
    return  !l_isfalse(pV);
#else
    bvalue(pV)>0
#endif
}

void CLuaMessageTableOptInC::GetKeyFromNode(const Node* pNode, TValue& pV)
{
    if(pNode == nullptr)
        return;
#if 504 == LUA_VERSION_NUM
    pV.value_ = pNode->u.key_val;
    pV.tt_ = pNode->u.key_tt;
#else
#endif

}

bool CLuaMessageTableOptInC::NodeIsDataKeyIsNumberorString(const Node* pNode)
{
#if 504 == LUA_VERSION_NUM
    if(pNode == nullptr)
        return false;
    if(ttisnil(&pNode->i_val) || novariant(pNode->u.key_tt) == LUA_TNIL)
        return false;
    if(novariant(pNode->u.key_tt) != LUA_TNUMBER && novariant(pNode->u.key_tt) != LUA_TSTRING)
        return false;
    return true;
#else
    if(pNode == nullptr)
        return false;
    if(ttisnil(&pNode->i_key.nk) || ttisnil(&pNode->i_val) || ttisfunction(&pNode->i_val))
        return false;
    if(!ttisnumber(&pNode->i_key.tvk) && !ttisstring(&pNode->i_key.tvk))
        return  false;
    return  true;
#endif

}