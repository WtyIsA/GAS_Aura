#pragma once

#include "lua.hpp"
#include <string>
#include "Net/Marshal.h"
#include "Net/UnMarshal.h"

#define LUA_MSG_TYPE_BOOL  1
#define LUA_MSG_TYPE_BYTE  2
#define LUA_MSG_TYPE_SHORT  3
#define LUA_MSG_TYPE_INT  4
#define LUA_MSG_TYPE_LONG  5
#define LUA_MSG_TYPE_FLOAT  6
#define LUA_MSG_TYPE_DOUBLE  7
#define LUA_MSG_TYPE_STRING  8
#define LUA_MSG_TYPE_LIST  9    // 数组后面跟类型
#define LUA_MSG_TYPE_SET  10
#define LUA_MSG_TYPE_MAP  11     // MAP后面跟 key ,value
#define LUA_MSG_TYPE_OCTETS  12
#define LUA_MSG_TYPE_TABLE  20  //20以上的设置为table，为各种协议和bin

#define LUA_MSG_MSG 0  //消息
#define LUA_MSG_RPC 1  //RPC
#define LUA_MSG_BEAN 2  //BEAN

class  USessionMgr;

struct SLuaMsgInfo
{
    SLuaMsgInfo(const FString& formatInfo,const FString& name, int8 nType)
    {
        m_msgName = TCHAR_TO_ANSI(*name);
        AnalysisMsg(formatInfo,m_subTypes);
        m_nVariableCount = 1;
        m_nType = nType;
        m_pSessionMgr = nullptr;
        GetMsgVariableCount(m_subTypes, m_nVariableCount);
    }
    virtual ~SLuaMsgInfo()
    {
        UE_LOG(LogTemp,Display,TEXT("~SLuaMsgInfo call"))
    }

    virtual bool Marshal(Table* table, FMarshal* obs);
    virtual bool UnMarshal(FUnMarshal* ibs, Table* table);
    virtual bool IsRpc()const {return false;}
    virtual int  GetMsgLen();
    int8         GetMsgType(){return m_nType;} 
    void         SetManager(USessionMgr* pMgr){m_pSessionMgr = pMgr;}
    const USessionMgr* GetManager() const{return  m_pSessionMgr;}
protected:
    bool MarshalArray(Table* table, FMarshal* obs,const TArray<int>& array, int tabStartPos = 0);
    void AnalysisMsg(const FString& formatInfo, TArray<int>& array );
    bool MarshalMap(Table* table, FMarshal* obs, int KeyType, int ValueType);
    bool MarshalList(Table* table, FMarshal* obs, int ValueType);
    bool MarshalSet(Table* table, FMarshal* obs, int KeyType);
    bool MarshaBaseType(TValue* ptvalue, FMarshal* obs, int ValueType);
    bool MarshaBaseType(int key, FMarshal* obs, int ValueType);
    template <typename T> bool MarshalBaseType(T a, FMarshal* obs);

    bool UnMarshalArray(FUnMarshal* ibs, Table* table,const TArray<int>& array, int tabStartPos = 0);
    bool UnMarshalList(FUnMarshal* ibs, Table* table, int nArraySize ,int ValueType);
    bool UnMarshalMap(FUnMarshal* ibs, Table* table, int KeyType ,int ValueType);
    bool UnMarshalSet(FUnMarshal* ibs, Table* table, int KeyType);

    //nType: List 和 Set 对应 bin的类型， map：key 或者 value类型
    bool UnMarshalTValue(FUnMarshal* ibs, int nType, TValue& retValue);
    template <typename T> bool UnMarshalBaseType(T& result, FUnMarshal* ibs);

    void GetMsgVariableCount(const TArray<int>& array, uint8& nCount);
public:
    std::string                 m_msgName;
    TArray<int>                 m_subTypes;
    uint8                       m_nVariableCount;
protected:
    USessionMgr*                m_pSessionMgr;
    uint8                       m_nType;
};
