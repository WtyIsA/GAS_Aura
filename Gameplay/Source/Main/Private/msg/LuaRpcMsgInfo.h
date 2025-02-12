#pragma once
#include "SLuaMsgInfo.h"

struct SLuaRpcMsgInfo : public SLuaMsgInfo
{
	SLuaRpcMsgInfo(const FString& formatInfo,const FString& replyInfo,const FString& name);
	virtual ~SLuaRpcMsgInfo(){};
	virtual bool IsRpc()const {return true;}
	virtual bool Marshal(Table* table, FMarshal* obs);
	virtual bool UnMarshal(FUnMarshal* ibs, Table* table);
	virtual int  GetMsgLen(){return m_resultVariableCount + m_nVariableCount + 4;}
protected:
	
public:
	TArray<int>		m_resultSubTypes;
	uint8			m_resultVariableCount;

	static int SIGN_REQUEST;
	static int SIGN_EXCEPTION;
	static int SIGN_RELIABLE;
	static int SIGN_PERSIST;
};

struct SLRpcInfo
{
	SLRpcInfo(lua_State* nl, Table* ptable)
	{
		L = nl;
		table = ptable;
	}
	SLRpcInfo(const SLRpcInfo& other)
	{
		L= other.L;
		table = other.table;
	}
	Table* table;
	lua_State* L;
};