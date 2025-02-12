#include "LuaRpcMsgInfo.h"

#include "LuaManger.h"
#include "LuaMessageTableOptInC.h"
#include "UnLuaBase.h"
#include "Net/SessionMgr.h"


int SLuaRpcMsgInfo::SIGN_REQUEST = 0x01;
int SLuaRpcMsgInfo::SIGN_EXCEPTION = 0x02;
int SLuaRpcMsgInfo::SIGN_RELIABLE = 0x04;
int SLuaRpcMsgInfo::SIGN_PERSIST = 0x08;

SLuaRpcMsgInfo::SLuaRpcMsgInfo(const FString& formatInfo,const FString& replyInfo,const FString& name) : SLuaMsgInfo(formatInfo, name, LUA_MSG_RPC)
{
	AnalysisMsg(replyInfo, m_resultSubTypes);
	GetMsgVariableCount(m_resultSubTypes, m_resultVariableCount);
}


bool SLuaRpcMsgInfo::Marshal(struct Table* table, FMarshal* obs)
{
	int64 nId = ivalue(&table->array[2]);
	int8 nSigns = ivalue(&table->array[3]);
	MarshalBaseType(nId, obs);
	MarshalBaseType(nSigns,obs);
	if(nSigns & SIGN_REQUEST)
		return MarshalArray(table, obs, m_subTypes);
	if((nSigns & SIGN_EXCEPTION) == 0)
		return MarshalArray(table, obs, m_resultSubTypes , m_nVariableCount);
	else
	{
		return true;
	}
}

bool SLuaRpcMsgInfo::UnMarshal(FUnMarshal* ibs, Table* table)
{
	int64 nId = 0;
	if(!UnMarshalBaseType(nId, ibs))
		return false;
	int8 nSigns = 0;
	if(!UnMarshalBaseType(nSigns, ibs))
		return false;
	TValue uValue ;
	TArray<int>* subType = &m_subTypes;
	int tabStartPos = 0;
	if(nSigns & SIGN_REQUEST)
	{
		table = m_pSessionMgr->NewRpcTable(UnLua::GetState(), this);
		if(table == nullptr)
			return false;
		setivalue(&uValue, nId);
		CLuaMessageTableOptInC::AddTValue(table, 3, &uValue);
	}
	else
	{
		SLRpcInfo* pRpcInfo = m_pSessionMgr->FindRpc(nId);
		if (pRpcInfo == nullptr)
		{
			UE_LOG(GAME, Error, TEXT("[net]recv rpc SIGN_EXCEPTION,rpcid %d error"), nId);
			return false;
		}
		table = pRpcInfo->table;
		CLuaMessageTableOptInC::PushTable(table, UnLua::GetState());
		tabStartPos = m_nVariableCount;
		subType = &m_resultSubTypes;
	}
	setivalue(&uValue, nSigns);
	CLuaMessageTableOptInC::AddTValue(table, 4, &uValue);
	
	if ((nSigns & SIGN_EXCEPTION) != 0)
	{
		return true;
	}
	return UnMarshalArray(ibs, table, *subType, tabStartPos);
}
