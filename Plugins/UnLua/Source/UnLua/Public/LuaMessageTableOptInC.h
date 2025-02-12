#pragma once
#include "lua.hpp"


class UNLUA_API CLuaMessageTableOptInC
{
private:
    CLuaMessageTableOptInC();
public:
    static  CLuaMessageTableOptInC* Get();
    static Table* CreateNewTable(const char* metatablename, int nArraySize = 1, int nHashSize = 1, lua_State *Nl = nullptr, bool bPushToTop = false);
    static bool SetTableMetatable(Table* pTable,const char* metatablename, lua_State *Nl=nullptr);
    static void IncreaseTableArrays(Table* pTable,int nNewArraySize, lua_State *Nl=nullptr);
    static Udata* CreateUsedata(int len,  int nuvalue, void* pMem,  lua_State *Nl=nullptr);

    //nIndex从1开始
    static void AddTValue(Table* table, int nIndex, TValue* pV, lua_State *Nl = nullptr);
    static void AddTValueByKey(Table* table,unsigned int nIndex, TValue* pV, lua_State *Nl = nullptr);
    //nIndex从1开始
    static void AddTable(Table* table, int nIndex, Table* subTable, lua_State *Nl = nullptr);
    static void AddTable(Table* table,const char* strKey, Table* subTable, lua_State *Nl = nullptr);    
    static void AddValue(Table* table, const char* strKey, int n, lua_State *Nl = nullptr);
    static void AddValue(Table* table, TValue* pKey, TValue*  pV, lua_State *Nl = nullptr);
    static void AddNode(Table* table,Node* pNode, lua_State *Nl = nullptr);
    static void AddThread(Table* table,lua_State *thread,lua_State *L);

    static bool RemoveValue(Table* table,const char* strKey, lua_State *Nl = nullptr);
    static bool RemoveValue(Table* table,int key, lua_State *Nl = nullptr);
    static bool RemoveValue(Table* table,TValue* pKey, lua_State *Nl = nullptr);

    static void PushTable(Table* table, lua_State *Nl = nullptr);
    static void PushValue(TValue* pValue, lua_State *Nl = nullptr);
    static void PushUserdata(Udata* ud, lua_State *Nl = nullptr);
    static void PushLuaFunction(const void* pFun,lua_State *Nl = nullptr);
    
    static const TValue* GetTValue(const Table* table,int64 nKey, lua_State *Nl = nullptr);
    static const TValue* GetTValue(const Table* table,FString& strKey, lua_State *Nl = nullptr);
    static const TValue* GetTValue(const Table* table,const char* strKey, lua_State *Nl = nullptr);
    static int GetNumber(const Table* table,const char* strKey, lua_State *Nl = nullptr);
    static void GetNumber(const Table* table,const char* strKey, int&nValue, lua_State *Nl = nullptr);
    static void GetNumber(const Table* table,const char* strKey, uint8&nValue, lua_State *Nl = nullptr);
    static Table* GetTable(const Table* table,const char* strKey, lua_State *Nl = nullptr);

    static lua_State* CreateState();
    static void FreeState(lua_State* L);

    static int PrintLuaTrackbackAndPushNil(const char* errorMsg = nullptr, lua_State *Nl = nullptr, int level =1);
    static int OnlyPrintLuaTrackback(const char* errorMsg = nullptr, lua_State *Nl = nullptr, int level =1);

    static const char* GetTValueStr(const TValue* pValue);
    static const char* GetTValueStr(const Value* pValue);
    static const char* GetTValueStr(const Table* table,const char* strKey, lua_State *Nl = nullptr);

    static bool SetValue(Table* table,const char* strKey, lua_Integer n, bool bAddIfNotExist = true, lua_State *Nl = nullptr);
    static void SetBoolValue(TValue* pV, bool var);
    static bool GetBoolValue(const TValue* pV);
    static void GetKeyFromNode(const Node* pNode, TValue& pV);
    
    static bool NodeIsDataKeyIsNumberorString(const Node* pNode);
private:
    static TValue* InsertToTableGetValue(Table* table,const char*strKey, lua_State *Nl = nullptr);
    static TValue* InsertToTableGetValue(Table* table, lua_Integer nKey, lua_State *Nl = nullptr);

    
};

