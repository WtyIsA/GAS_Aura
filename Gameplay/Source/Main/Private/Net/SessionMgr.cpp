#include "SessionMgr.h"
#include "lua.hpp"
#include "LuaManger.h"
#include "LuaMessageTableOptInC.h"
#include "MemTracker.h"
#include "MessagePool.h"
#include "SessionClient.h"
#include "SessionPingClient.h"
#include "SessionServer.h"
#include "UnLuaBase.h"
#include "UnLuaEx.h"
#include "Http/HttpRequest.h"
#include "msg/LuaRpcMsgInfo.h"
#include "msg/SLuaMsgInfo.h"

bool USessionMgr::m_bShowMessageLog = true;
int32 USessionMgr::m_rpcId = 1;

USessionMgr::USessionMgr(): m_waitTime(FTimespan::FromMilliseconds(10))
{
#ifndef  PLATFORM_HAS_BSD_SOCKETS	
	UE_LOG(GAME,Error,TEXT("[net]USessionMgr,not define PLATFORM_HAS_BSD_SOCKETS"));
	check(false);
#endif

	m_stopping = false;
	m_thread = nullptr;
	m_maxfd = 1;
	m_selectInterrupter = nullptr;
#if UE_BUILD_SHIPPING
	m_bShowMessageLog = false;
#else
	if(IsRunningDedicatedServer())
		m_bShowMessageLog = false;
#endif
	m_pNotifyFun = nullptr;
	m_pSendOverrideFun = nullptr;
	SLuaMsgInfo* pMsg = new SLuaMsgInfo(TEXT(""), TEXT("ArrayList"), 2);
	m_mapLuaMsgs.Add(9, pMsg);
	pMsg = new SLuaMsgInfo(TEXT(""), TEXT("HashSet"), 2);
	m_mapLuaMsgs.Add(10, pMsg);
	pMsg = new SLuaMsgInfo(TEXT(""), TEXT("HashMap"), 2);
	m_mapLuaMsgs.Add(11, pMsg);
	m_bCreateRealThread = true;
}

FString USessionMgr::GetModuleName_Implementation() const
{
	FString name = GetName();
	name.RemoveFromStart(DEFAULT_OBJECT_PREFIX);
	if(IsRunningDedicatedServer())
	{
		return FString::Printf(TEXT("Server.Net.%s"), *name);
	}
	else
	{
		return FString::Printf(TEXT("Client.Net.%s"), *name);
	}
}


void USessionMgr::Start()
{
	if(m_thread != nullptr)
		return;
	m_selectInterrupter = new CSelectInterrupter();
    m_selectInterrupter->OpenInterrupter();
	m_thread = FRunnableThread::Create(this, TEXT("SessionThread"));	
	m_maxfd = FMath::Max(m_maxfd, m_selectInterrupter->GetSocket());

	m_bCreateRealThread = FPlatformProcess::SupportsMultithreading();
#if PLATFORM_LINUX
	if(IsRunningDedicatedServer() && FCommandLine::IsInitialized())
	{
		m_bCreateRealThread = !FParse::Param(FCommandLine::Get(), TEXT("nothreading"));
	}
#endif
}

void USessionMgr::Stop()
{
	if(m_thread == nullptr)
		return;
	m_stopping = true;
	m_thread->WaitForCompletion();
	m_thread = nullptr;
}

void USessionMgr::BeginDestroy()
{
	Stop();
	for(auto item :m_sessions)
	{
		item->RemoveFromRoot();
		item->Close();
	}
	USession* pAddSession = nullptr;
	while (m_addSessions.Dequeue(pAddSession))
	{
		if(pAddSession)
		{
			pAddSession->RemoveFromRoot();
			pAddSession->Close();
		}
	}
	if(m_selectInterrupter)
	{
		delete m_selectInterrupter;
		m_selectInterrupter = nullptr;
	}
	Super::BeginDestroy();
}

bool USessionMgr::RegisterMessage(void* msgTable)
{
	if(msgTable == nullptr)
		return false;
	Table* allMsg = (Table*)msgTable;
	unsigned int nCount = luaH_realasize(allMsg);
	for(unsigned int i = 0; i < nCount; i++)
	{
		TValue* pTempValue = &allMsg->array[i];
		if(!ttistable(pTempValue))
			continue;
		Table* singleMsg = hvalue(pTempValue);
		if(singleMsg->alimit < 4 || !ttisinteger(&singleMsg->array[0])|| !ttisinteger(&singleMsg->array[1]))
		{
			UE_LOG(GAME,Error,TEXT("[msg]msg table len error %d"), i);
			return false;
		}		
		int32 msgId = ivalue(&singleMsg->array[0]);
		if(m_mapLuaMsgs.Contains(msgId))
		{
			SLuaMsgInfo* pMsg = m_mapLuaMsgs.FindRef(msgId);
			check(pMsg);
			delete pMsg;
			m_mapLuaMsgs.Remove(msgId);
		}		
		int8 nType = ivalue(&singleMsg->array[1]);
		if(nType < LUA_MSG_MSG || nType > LUA_MSG_BEAN)
		{
			UE_LOG(GAME,Error,TEXT("[msg]msg type value error %d"), i);
			return false;
		}
		SLuaMsgInfo* pMsg = nullptr;
		if(nType == LUA_MSG_RPC)
		{
			if(singleMsg->alimit != 5 || !ttisstring(&singleMsg->array[2])
				|| !ttisstring(&singleMsg->array[3])|| !ttisstring(&singleMsg->array[4]))
			{
				UE_LOG(GAME,Error,TEXT("[msg]rpc info error %d"), i);
				return false;
			}
			pMsg = new SLuaRpcMsgInfo(UTF8_TO_TCHAR(svalue(&singleMsg->array[2])),
				UTF8_TO_TCHAR(svalue(&singleMsg->array[3])),
				UTF8_TO_TCHAR(svalue(&singleMsg->array[4])));
		}
		else
		{
			if(singleMsg->alimit != 4 || !ttisstring(&singleMsg->array[2])
				|| !ttisstring(&singleMsg->array[3]))
			{
				UE_LOG(GAME,Error,TEXT("[msg]rpc info error %d"), i);
				return false;
			}
			pMsg = new SLuaMsgInfo(UTF8_TO_TCHAR(svalue(&singleMsg->array[2])),
				UTF8_TO_TCHAR(svalue(&singleMsg->array[3])), nType);
		}
		pMsg->SetManager(this);
		m_mapLuaMsgs.Add(msgId, pMsg);
		
	}
	return true;
}


void USessionMgr::Tick()
{
	if (!m_bCreateRealThread)
		Update();
	for(int i = m_sessions.Num() - 1; i >= 0; i--)
	{
		auto item = m_sessions[i];
		if(item && item->GetConnectState() != SessionState_Remove)
		{
			item->Tick();
		}
	}
}

void USessionMgr::RemoveSession(USession* pSession)
{
	if(pSession == nullptr)
	{
		UE_LOG(GAME,Warning,TEXT("[net]RemoveSession null"));
		return;
	}
	pSession->Close();
	pSession->SetConnectState(SessionState_Remove);
	m_sessions.Remove(pSession);
	m_removeSessions.Enqueue(pSession);
}

UObject* USessionMgr::CreateSession(const FString& strHost, uint32 port, int32 connectTimeOut)
{
	USessionClient* pSession = NewObject<USessionClient>();
	if(pSession->Connect(strHost, port, connectTimeOut))
	{
		AddSession(pSession);
		return pSession;
	}
	pSession->MarkPendingKill();
	return nullptr;
}

UObject*  USessionMgr::CreatePingSession(const FString& strIp, int port)
{
	USessionPingClient* pSession = NewObject<USessionPingClient>();
	if(!pSession->InitSocket())
	{
		pSession->MarkPendingKill();
		return nullptr;
	}
	AddSession(pSession);
	if(strIp.Len() > 4)
		pSession->Ping(strIp, port);
	return pSession;
}


UObject* USessionMgr::Listen(const FString& strHost, uint32 port, uint32 nMaxCount, uint32 connectedAliveTimeout)
{
	USessionServer* pSession = NewObject<USessionServer>();
	if(pSession->Listen(strHost, port, nMaxCount, connectedAliveTimeout))
	{
		AddSession(pSession);
		return pSession;
	}
	pSession->MarkPendingKill();
	return nullptr;
}


void USessionMgr::AddSession(USession* pSession)
{
	if(pSession == nullptr)
	{
		UE_LOG(GAME,Warning,TEXT("[net]AddSession null"));
		return;
	}	
	pSession->SetManager(this);
	pSession->AddToRoot();
	m_sessions.Add(pSession);
	m_addSessions.Enqueue(pSession);	
	m_maxfd = FMath::Max(m_maxfd, pSession->GetSocket());
}

bool USessionMgr::CheckSessionInSelectList(const USession* pSession,const TArray<USession*>& sessionList)
{
	for(auto item:sessionList)
	{
		if(item == pSession)
			return true;
	}
	return false;
}

uint32 USessionMgr::Run()
{
	while (!m_stopping)
	{
		Update();
	}
	return 0;
}

bool USessionMgr::Update()
{	
	timeval Time;
	Time.tv_sec = (int32)m_waitTime.GetTotalSeconds();
	Time.tv_usec = m_waitTime.GetFractionMicro();
	timeval* TimePointer = m_waitTime.GetTicks() >= 0 ? &Time : nullptr;
	FD_ZERO(&m_writeset);
	FD_ZERO(&m_readset);

	USession* pSession = nullptr;
	while (m_addSessions.Dequeue(pSession))
	{
		if(pSession && pSession->SocketIsValid()
			&& !CheckSessionInSelectList(pSession, m_netThreadSessionList))
		{			
			m_netThreadSessionList.Add(pSession);
		}
	}
	while (m_removeSessions.Dequeue(pSession))
	{
		if(pSession)
		{			
			m_netThreadSessionList.Remove(pSession);
			pSession->RemoveFromRoot();
		}
	}
	if(m_selectInterrupter)
		FD_SET(m_selectInterrupter->GetSocket(), &m_readset);
	for(int i = 0; i < m_netThreadSessionList.Num(); i++)
	{
		auto item = m_netThreadSessionList[i];
		if(item == nullptr)
		{
			UE_LOG(GAME,Warning,TEXT("[net]session is null, %d, size=%d"), i-1, m_netThreadSessionList.Num());
			continue;
		}
		switch (item->GetConnectState())
		{
		case SessionState_Connecting:
			FD_SET(item->GetSocket(), &m_readset);
			FD_SET(item->GetSocket(), &m_writeset);
			m_maxfd = FMath::Max(m_maxfd, item->GetSocket());
			break;
		case SessionState_Connected:
			FD_SET(item->GetSocket(), &m_readset);
			if(item->HasDataToSend())
				FD_SET(item->GetSocket(), &m_writeset);
			break;
		case SessionState_Listen:
			FD_SET(item->GetSocket(), &m_readset);
			break;
		default:
			break;
		}
	}
	int32 SelectStatus = select(m_maxfd + 1, &m_readset, &m_writeset,nullptr,TimePointer);
	if(SelectStatus > 0)
	{
		if(m_selectInterrupter && FD_ISSET(m_selectInterrupter->GetSocket(), &m_readset))
		{
			m_selectInterrupter->RecvData();
		}
		for(int i = m_netThreadSessionList.Num(); i > 0; i--)
		{
			auto item = m_netThreadSessionList[i-1];
			if(item == nullptr)
			{
				//UE_LOG(GAME,Warning,TEXT("[net]session is null, %d"), i-1);
				continue;
			}
			if(!item->SocketIsValid())
			{
				//m_sessions.erase(m_sessions.begin() + i-1);
				continue;
			}
			bool bInReadSet = FD_ISSET(item->GetSocket(), &m_readset) > 0;
			bool bInWriteSet = FD_ISSET(item->GetSocket(), &m_writeset) > 0;
			if(item->GetConnectState() == SessionState_Connecting)
			{
				if(bInWriteSet)
				{
					bool bConnect = false;
					if(bInReadSet)
					{
						int nError = 0;
						socklen_t optLen = sizeof(nError);
						int flag = getsockopt(item->GetSocket(),SOL_SOCKET, SO_ERROR, (char*)&nError,&optLen);
						if(flag == 0 && nError == 0)
						{
							bConnect = true;
						}					
					}
					else
					{
						bConnect = true;
					}
					item->SetConnectState(bConnect?SessionState_Connected:SessionState_ConnectFailed);
				}
				continue;
			}
			if(bInWriteSet)
			{
				item->Send();
			}
			if(bInReadSet)
			{
				item->Recv();
			}
		}
	}
	return true;
}

void USessionMgr::InterruptSelect()
{
	if(m_selectInterrupter)
		m_selectInterrupter->Interrupt();
}

USession* USessionMgr::GetSessionById(int32 sid)
{
	for (auto item : m_sessions)
	{
		if(item->GetSessionId() == sid)
			return item;
	}
	return nullptr;
}

void USessionMgr::ProcessRecvMsg(int64 sid,FUnMarshal* ibs, int32 msgId, int32 Length , const char* clientId,const void* luaFun)
{
	if (Length != ibs->Length())
	{
		UE_LOG(GAME, Error, TEXT("[msg]recv msg:%d, msglen:%d, oclen=%d"), msgId, Length, ibs->Length());
		return;
	}
	
	if (!m_mapLuaMsgs.Contains(msgId))
	{
		UE_LOG(GAME, Error, TEXT("[msg]recv msg error, msgid %d not exist"), msgId);
		return;
	}
	SLuaMsgInfo* pInfo = m_mapLuaMsgs.FindRef(msgId);
	int8 nMsgType = pInfo->GetMsgType();
	
	if (m_bShowMessageLog)
	{
		UE_LOG(GAME, Display, TEXT("[msg]recv msg id=%d, len=%d, name=%s"), msgId, Length,
			   ANSI_TO_TCHAR( pInfo->m_msgName.c_str()));
	}

	lua_State* L= UnLua::GetState();
	if(nMsgType == LUA_MSG_MSG)
	{
		int arrayLen = pInfo->GetMsgLen();
		Table* pTable = CreateTable(msgId, arrayLen, 2, true);
		FillMessageHead(pTable, sid, nMsgType);
		if(pInfo->UnMarshal(ibs, pTable))
		{
			if(clientId == nullptr)
				Notify(sid, pTable, msgId);
			else
				Notify(clientId, pTable, msgId, luaFun);
		}
		else
		{
			UE_LOG(GAME, Error, TEXT("[msg]recv msg unmarshal error %d"), msgId);        
		}
		lua_pop(L, 1);
	}
	else if(nMsgType == LUA_MSG_RPC)
	{
		if(pInfo->UnMarshal(ibs, nullptr))
		{
			Table* pTable = (Table*)lua_topointer(L, -1);
			int8 nSign = ivalue(&pTable->array[3]);
			if(nSign & SLuaRpcMsgInfo::SIGN_REQUEST)
			{
				FillMessageHead(pTable, sid, nMsgType);
				if(clientId == nullptr)
					Notify(sid, pTable, msgId);
				else
					Notify(clientId, pTable, msgId, luaFun);
			}
			else
			{
				int32 nrpcId = ivalue(&pTable->array[2]);
				SLRpcInfo* rpcInfo = FindRpc(nrpcId);
				if(rpcInfo)
				{
					int nResult = 0;
					lua_resume(rpcInfo->L, L,0, &nResult);
					m_rpcList.Remove(nrpcId);
				}
				
			}
			lua_pop(L, 1);
		}
		else
		{
			UE_LOG(GAME, Error, TEXT("[msg]recv msg unmarshal error %d"), msgId);        
		}
	}
	else
	{
		UE_LOG(GAME, Error, TEXT("[msg]recv msg:%d, type error:%d should be 0 or 1"), msgId, nMsgType);		
	}
}

void USessionMgr::RecvMsg(int64 sid,FUnMarshal* ibs)
{
	MemTrackerScope(UnLua::ELuaLLMTag::MsgTag);
	if(ibs == nullptr)
		return;
	bool bUnmarshal = true;
	int Length = ibs->Read<int32>(bUnmarshal);
	int msgId = ibs->Read<int32>(bUnmarshal);		
	
	ProcessRecvMsg(sid, ibs, msgId, Length, nullptr, nullptr);
}

void USessionMgr::FillMessageHead(Table* pTable, int64 sid, int nMsgType)
{
	if(pTable == nullptr)
		return;
	lua_State* L= UnLua::GetState();
	TValue uValue ;
	sethvalue(L, &uValue, m_bindLuaTable);
	CLuaMessageTableOptInC::AddTValue(pTable,  1, &uValue);
	setivalue(&uValue, sid);
	CLuaMessageTableOptInC::AddTValue(pTable,  2, &uValue);
}


void USessionMgr::RecvMsg(int64 sid, FUnMarshal* ibs, uint32 forwardId, const void* notifyFun)
{
    MemTrackerScope(UnLua::ELuaLLMTag::MsgTag);
    if(ibs == nullptr)
        return;
    bool bUnmarshal = true;
    int Length = ibs->Read<int32>(bUnmarshal);
    int msgId = ibs->Read<int32>(bUnmarshal);
    if(msgId == forwardId)
    {
        std::string clientId;
        ibs->ReadStr(clientId,bUnmarshal);
        int octestLen = ibs->Read<int32>(bUnmarshal);
        if(ibs->Length() != octestLen + ibs->GetPos())
        {
            UE_LOG(GAME,Error,TEXT("CLuaMsgMgr::RecvMsg forward len error, alllen=%d,octestsLen=%d"), ibs->Length(), octestLen);
            return;
        }
        FUnMarshal* msgUnMarshal = CMessagePool::Get()->GetUnMarshal(Length - ibs->GetPos());                 
        msgUnMarshal->PushBuffer(ibs->GetData()+ibs->GetPos(), octestLen);
        Length = msgUnMarshal->Read<int32>(bUnmarshal);
        msgId = msgUnMarshal->Read<int32>(bUnmarshal);
        ibs = msgUnMarshal;
    	ProcessRecvMsg(sid, ibs, msgId, Length, clientId.c_str(), notifyFun);  
    }
    else
    {
    	ProcessRecvMsg(sid, ibs, msgId, Length, nullptr, nullptr);  
    }    
}

Table* USessionMgr::CreateTable(int msgId, int nArraySize, int nHashSize, bool bPushToTop)
{
	SLuaMsgInfo* msgInfo = m_mapLuaMsgs.FindRef(msgId);
	if(msgId > LUA_MSG_TYPE_TABLE && msgInfo != nullptr && nArraySize < msgInfo->m_nVariableCount)
	{
		nArraySize = msgInfo->m_nVariableCount;
	}
	Table* pTable = CLuaMessageTableOptInC::CreateNewTable(msgInfo?msgInfo->m_msgName.c_str():nullptr, nArraySize, nHashSize,UnLua::GetState(), bPushToTop);
	return  pTable;
}

bool USessionMgr::UnMarshal(int64 sid, FUnMarshal* ibs, Table* table, int msgId)
{
	if(table == nullptr || ibs == nullptr)
		return false;
	if(!m_mapLuaMsgs.Contains(msgId))
	{
		UE_LOG(GAME, Error, TEXT("[msg]msg id not exist:%d"), msgId);
		return  false;
	}
	SLuaMsgInfo* msgInfo = m_mapLuaMsgs.FindRef(msgId);
	return  msgInfo->UnMarshal(ibs, table);
}

void USessionMgr::Notify(const char* clientId, Table* pTable, int msgId, const void* luaFun)
{
    if(luaFun == nullptr || pTable == nullptr || clientId == nullptr)
        return;
    lua_State *L = UnLua::GetState();
	if(luaFun)
	{
		CLuaMessageTableOptInC::PushLuaFunction(luaFun, L);
		lua_pushstring(L, clientId);
		CLuaMessageTableOptInC::PushTable(pTable,L);
		if(lua_pcall(L, 2, 0, 0) != LUA_OK)
		{
			UE_LOG(GAME, Warning,TEXT("[msg]forward message Notify error, msgId=%d"), msgId);
			UnLua::ReportLuaCallError(L);
		}
	}
	else
	{
		if(!m_mapLuaMsgs.Contains(msgId))
		{
			return;
		}
		SLuaMsgInfo* pMsg = m_mapLuaMsgs.FindRef(msgId);
		int32 Type = lua_getglobal(L,pMsg->m_msgName.c_str());
		if(Type == LUA_TTABLE)
		{        
			Type = lua_getfield(L, -1, "Notify");
			lua_pushstring(L, clientId);
			CLuaMessageTableOptInC::PushTable(pTable);
			if(Type == LUA_TFUNCTION)
			{
				if(lua_pcall(L, 2, 0, 0) != LUA_OK)
				{
					UE_LOG(GAME, Warning,TEXT("[msg]msg error, id=%d"), msgId);
					UnLua::ReportLuaCallError(L);
				}
			}
			else
			{
				UE_LOG(GAME, Display, TEXT("[msg]Notify not find"));
			}
			lua_pop(L, 1);     //pop    lua_getglobal(L,m_notifyClassName.c_str());
		}
	}
}


void USessionMgr::Notify(int64 sid, Table* pTable, int msgId)
{
	if(pTable == nullptr)
	{
		UE_LOG(GAME,Error,TEXT("[msg]notify,error,pTable=null"))
		return;
	}
	lua_State *L = UnLua::GetState();
	if(m_pNotifyFun != nullptr)
	{
		CLuaMessageTableOptInC::PushLuaFunction(m_pNotifyFun, L);
		lua_pushinteger(L, sid);
		CLuaMessageTableOptInC::PushTable(pTable,L);
		if(lua_pcall(L, 2, 0, 0) != LUA_OK)
		{
			UE_LOG(GAME, Warning,TEXT("[msg]forward message Notify error, msgId=%d"), msgId);
			UnLua::ReportLuaCallError(L);
		}
		return;
	}
	else
	{
		if(!m_mapLuaMsgs.Contains(msgId))
		{
			return;
		}
		SLuaMsgInfo* pMsg = m_mapLuaMsgs.FindRef(msgId);
		int32 Type = lua_getglobal(L,pMsg->m_msgName.c_str());
		if(Type == LUA_TTABLE)
		{        
			Type = lua_getfield(L, -1, "Notify");
			if(Type == LUA_TFUNCTION)
			{
				lua_pushinteger(L, sid);
				CLuaMessageTableOptInC::PushTable(pTable);
				if(lua_pcall(L, 2, 0, 0) != LUA_OK)
				{
					UE_LOG(GAME, Warning,TEXT("[msg]msg error, id=%d"), msgId);
					UnLua::ReportLuaCallError(L);
				}
			}
			else
			{
				UE_LOG(GAME, Display, TEXT("[msg]Notify not find"));
			}
			lua_pop(L, 1);     //pop    lua_getglobal(L,m_notifyClassName.c_str());
		}
	}
}

bool USessionMgr::Notify(Table* pTable, int nType, const void* luaFun)
{
	if(luaFun == nullptr)
		return false;
	lua_State *L = UnLua::GetState();
	CLuaMessageTableOptInC::PushLuaFunction(luaFun, L);
	lua_pushinteger(L, nType);
	if (pTable)
		CLuaMessageTableOptInC::PushTable(pTable, L);
	else
		lua_pushnil(L);
	if (lua_pcall(L, 2, 0, 0) != LUA_OK)
	{
		UE_LOG(GAME, Warning, TEXT("[msg]call common notiy error, id=%d"), nType);
		UnLua::ReportLuaCallError(L);
		return false;
	}

	return true;
}

Table* USessionMgr::NewRpcTable(lua_State* L, SLuaMsgInfo*  pMsg)
{	
	if(pMsg == nullptr)
		return nullptr;
	int32 Type = lua_getglobal(L,pMsg->m_msgName.c_str());
	if(Type == LUA_TTABLE)
	{        
		Type = lua_getfield(L, -1, "New");
		if(Type == LUA_TFUNCTION)
		{
			lua_pushvalue(L,-2);
			lua_remove(L,-3);
			if(lua_pcall(L, 1, 1, 0) != LUA_OK)
			{
				UE_LOG(GAME, Warning,TEXT("[msg]NewRpcTable, id=%s"), UTF8_TO_TCHAR(pMsg->m_msgName.c_str()));
				UnLua::ReportLuaCallError(L);
				return nullptr;
			}
			return (Table*)lua_topointer(L, -1);			 
		}
		else
		{
			UE_LOG(GAME, Display, TEXT("[msg]NewRpcTable New not find"));
		}
		lua_pop(L, 1);     //pop    lua_getglobal(L,m_notifyClassName.c_str());
	}
	return nullptr;
}


bool USessionMgr::Marshal(struct Table* table,FMarshal* obs, int msgId)
{
	if(table == nullptr || obs == nullptr)
	{
		UE_LOG(GAME, Error, TEXT("[msg]Marshal table or obs is null:%d"), msgId);
		return false;
	}
	if(!m_mapLuaMsgs.Contains(msgId))
	{
		UE_LOG(GAME, Error, TEXT("[msg]msg id not exist:%d"), msgId);
		return  false;
	}
	SLuaMsgInfo* msgInfo = m_mapLuaMsgs.FindRef(msgId);
	return msgInfo->Marshal(table, obs);;
}

SLuaMsgInfo* USessionMgr::GetMsgInfo(int msgId)
{
	if(m_mapLuaMsgs.Contains(msgId))
		return m_mapLuaMsgs.FindRef(msgId);
	return nullptr;
}

void USessionMgr::Send(int32 nSessionId, int32 msgId, Table* msg)
{
	Send(nSessionId, msgId, msg, nullptr, nullptr, 0);
}


void USessionMgr::Send(int32 nSessionId, int32 msgId, Table* msg, lua_State *L, const void* luaFun, int otherSidIndex)
{	
	if(luaFun == nullptr)
	{
		USession* pSession = GetSessionById(nSessionId);
		if(!pSession)
		{
			UE_LOG(GAME,Error,TEXT("[net]SessionMgr_Send  sid(%d) not exist, %d"), nSessionId, __LINE__);
			return;
		}
		pSession->SendMsg(msgId, msg);
		return;
	}
	if(L == nullptr)
		L = UnLua::GetState();
	CLuaMessageTableOptInC::PushLuaFunction(luaFun, L);
	if(otherSidIndex > 0)
		lua_pushvalue(L, otherSidIndex);
	else
		lua_pushinteger(L, nSessionId);
	lua_pushinteger(L, msgId);
	CLuaMessageTableOptInC::PushTable(msg,L);	
	
	if(lua_pcall(L, 3, 0, 0) != LUA_OK)
	{
		UE_LOG(GAME, Warning,TEXT("[msg]USessionMgr:Send error %d"), __LINE__);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("",L);
	}
}


int  USessionMgr::GetMsgIdByName(FString& msgName)
{
	if(msgName.Len() == 0 || m_mapLuaMsgs.Num() == 0)
		return -1;
	char* pAnsiMsg = TCHAR_TO_ANSI(*msgName);
	for(auto& item:m_mapLuaMsgs)
	{
		if(item.Value->m_msgName == pAnsiMsg)
			return item.Key;
	}
	return -1;
}

void PostBeanCallback(USessionMgr* pMgr,int id,FString strUrl, FUnMarshal& ibs, int nType, const void* pFun ){
	if(pMgr == nullptr || pFun == nullptr)
	{
		UE_LOG(GAME,Error,TEXT("[net]PostBeanCallback USessionMgr(%p) or pFun(%p) is null %s"),pMgr,pFun, *strUrl);
	}
	Table* table = pMgr->RecvBean(id, ibs);
	if(table)
	{
		USessionMgr::Notify(table, nType, pFun);
	}
	else
	{
		USessionMgr::Notify(nullptr, nType, pFun);
	}
}

void USessionMgr::SendBean(FString url,int nType, FString msgName, void* msg, FString recvBeanName, const void* pFun)
{
	Table* pTable = (Table*)msg;    
	if(pTable != nullptr)
	{
		int msgId = GetMsgIdByName(msgName);
		int recvBeanId = GetMsgIdByName(recvBeanName);
		if(msgId < 0)
		{
			UE_LOG(GAME,Error,TEXT("[msg]CLuaMsgInterface::SendBean msgname not exist, %s,%s"),*msgName,*recvBeanName);
			return;
		}
		FMarshal *obs = CMessagePool::Get()->GetMarshal();
		obs->ResetPos(0);
		if(Marshal(pTable, obs, msgId))
		{
			FHttpClientRequest * login = new FHttpClientRequest(this, true);
			login->HttpPost(url, *obs, nType, PostBeanCallback, recvBeanId, pFun);
		}
		else
		{
			Notify( nullptr, nType, pFun);
		}       
	}
}

Table* USessionMgr::RecvBean(int msgId,FUnMarshal& ibs)
{
	MemTrackerScope(UnLua::ELuaLLMTag::MsgTag);
	if (!m_mapLuaMsgs.Contains(msgId))
	{
		UE_LOG(GAME, Error, TEXT("[msg]recv msg error, msgid %d not exist"), msgId);
		return nullptr;
	}
	Table* pTable = CreateTable(msgId, 1, 2, true);
	if (UnMarshal(0, &ibs, pTable, msgId))
	{
		  return pTable;
	}
	UE_LOG(GAME, Error, TEXT("[msg]recv msg error, msgid %d ,recv len=%d"), msgId, ibs.Length());
	return nullptr;
}

Table* USessionMgr::CreateRpc(lua_State *L)
{
	int32 msgid = lua_tointeger(L,2);
	SLuaMsgInfo* msgInfo = GetMsgInfo(msgid);
	if(msgInfo == nullptr)
	{
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("[net]SessionMgr_CreateRpc  msgid error",L);
		return nullptr;
	}
	Table* pRpcTable = NewRpcTable(L,msgInfo);
	if(pRpcTable == nullptr)
		return nullptr;
	lua_pushvalue(L,3);
	lua_rawseti(L,-2,2);//table[2]=sid
	TValue uValue ;
	sethvalue(L, &uValue, m_bindLuaTable);
	CLuaMessageTableOptInC::AddTValue(pRpcTable,  1, &uValue);	

	setivalue(&uValue, m_rpcId);
	CLuaMessageTableOptInC::AddTValue(pRpcTable,  3, &uValue, L);
	setivalue(&uValue, SLuaRpcMsgInfo::SIGN_REQUEST);
	CLuaMessageTableOptInC::AddTValue(pRpcTable,  4, &uValue, L);
	m_rpcList.Add(m_rpcId, SLRpcInfo(L,pRpcTable));
	m_rpcId++;
	if(m_rpcId == INT32_MAX)
		m_rpcId = 1;
	return pRpcTable;
}

SLRpcInfo* USessionMgr::FindRpc(int32 rpcId)
{
	return  m_rpcList.Find(rpcId);
}


void USessionMgr::CallRpc(lua_State* L, Table* pTable, int32 MsgId, int otherSidIndex)
{
	if(pTable == nullptr || pTable->alimit < 4)
	{
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("[net]CallRpc error", L);
		return;
	}
	if(UnLua::GetState() == L)
	{
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("[net]SessionMgr_CallRpc not in Coroutine",L);
		return ;
	}
	int32 nRpcId = ivalue(&pTable->array[2]);
	SLRpcInfo* rpcInfo = m_rpcList.Find(nRpcId);
	if(rpcInfo == nullptr)
	{
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("[net]SessionMgr_CallRpc id not exist",L);
		return ;
	}
	rpcInfo->L = L;
	int nSessionId = -1;
	if(ttisinteger(&pTable->array[1]))
		nSessionId =  ivalue(&pTable->array[1]);
	Send(nSessionId, MsgId,pTable, L, m_pSendOverrideFun, otherSidIndex);
	
	lua_yield(L,0);
}

bool USessionMgr::Marshal(int32 MsgId, Table* msg, TArray<uint8>& msgData)
{
	FMarshal* Obs = CMessagePool::Get()->GetMarshal();
	Obs->Write(MsgId);
	if(!Marshal(msg, Obs, MsgId))
	{
		CMessagePool::Get()->RecycleMessage(Obs);
		UE_LOG(GAME, Error,TEXT("[net]MarshalTable,error,msgid=%d"),MsgId);
		return false;
	}
	Obs->Pack();
	Obs->RemoveNotUser();
	msgData = Obs->GetBytes();
	CMessagePool::Get()->RecycleMessage(Obs);
	return true;
}

Table* USessionMgr::UnMarshal(int64 sId,const TArray<uint8>& msgData)
{
	FUnMarshal*  msgIbs = CMessagePool::Get()->GetUnMarshal();
	msgIbs->AttachArray(msgData,true);
	bool bUnmarshal = true;
	int Length = msgIbs->Read<int32>(bUnmarshal);
	int msgId = msgIbs->Read<int32>(bUnmarshal);
	if(!bUnmarshal || !m_mapLuaMsgs.Contains(msgId))
	{
		lua_pushnil(UnLua::GetState());
		CMessagePool::Get()->RecycleMessage(msgIbs);
		UE_LOG(GAME, Error, TEXT("[msg]UnMarshal msg error, msgid %d not exist"), msgId);
		return nullptr;
	}
	SLuaMsgInfo* pInfo = m_mapLuaMsgs.FindRef(msgId);
	CMessagePool::Get()->RecycleMessage(msgIbs);
	int arrayLen = pInfo->GetMsgLen();
	Table* pTable = CreateTable(msgId, arrayLen, 2, true);
	FillMessageHead(pTable, sId, pInfo->GetMsgType());
	if(!pInfo->UnMarshal(msgIbs, pTable))	
	{
		lua_State* L = UnLua::GetState();
		UE_LOG(GAME, Error, TEXT("[msg]UnMarshal msg  error %d"), msgId);
		lua_pop(L, 1);
		pTable = nullptr;
		lua_pushnil(L);
	}
	CMessagePool::Get()->RecycleMessage(msgIbs);

	return pTable;
	
}


int SessionMgr_ManagerBindLuaTable(lua_State *L)
{
	if(lua_gettop(L) < 2)
	{
		UE_LOG(GAME, Error,TEXT("ManagerBindLuaTable  param error."));
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("",L);
		return 0;
	}
	if(lua_type(L,1) != LUA_TTABLE || lua_type(L,2) != LUA_TTABLE)
	{
		UE_LOG(GAME, Error,TEXT("ManagerBindLuaTable  param type error."));
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("",L);
		return 0;
	}
	USessionMgr *pMgr = Cast<USessionMgr>(UnLua::GetUObject(L, 1));
	if(pMgr == nullptr)
	{
		UE_LOG(GAME, Error,TEXT("ManagerBindLuaTable  param1 error."));
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("",L);
		return 0;
	}
	Table* table = (Table*)lua_topointer(L,2);
	pMgr->BindLuaTable(table);
	return 0;
}

int SessionMgr_CallRpc(lua_State *L)
{
	if(lua_gettop(L) < 4 || lua_type(L,2) != LUA_TNUMBER || lua_type(L,3) != LUA_TTABLE)
	{
		UE_LOG(GAME, Error,TEXT("[net]SessionMgr_CallRpc  param error."));
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("",L);
		return 0;
	}
	
	USessionMgr *pMgr = Cast<USessionMgr>(UnLua::GetUObject(L, 1));
	if(pMgr == nullptr)
	{
		UE_LOG(GAME, Error,TEXT("[net]SessionMgr_CallRpc  param1 not USessionMgr."));
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("",L);
		return 0;
	}
	
	int32 msgid = lua_tointeger(L,2);
	Table* table = (Table*)lua_topointer(L,3);
	pMgr->CallRpc(L, table, msgid, 4 );
	return 0;
}


int SessionMgr_CreateRpc(lua_State *L)
{
	if(lua_gettop(L) < 3 || lua_type(L,2) != LUA_TNUMBER)
	{
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("[net]SessionMgr_CreateRpc  param error",L);
		return 0;
	}
	
	USessionMgr *pMgr = Cast<USessionMgr>(UnLua::GetUObject(L, 1));
	if(pMgr == nullptr)
	{
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("[net]SessionMgr_CreateRpc  param1 not USessionMgr",L);
		return 0;
	}
	
	Table* table = pMgr->CreateRpc(L);
	return table?1:0;
}

int SessionMgr_Send(lua_State *L)
{
	if(lua_gettop(L) < 4 || lua_type(L,3) != LUA_TNUMBER || lua_type(L,4) != LUA_TTABLE)
	{
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("[net]SessionMgr_Send  param error",L);
		return 0;
	}
	
	USessionMgr *pMgr = Cast<USessionMgr>(UnLua::GetUObject(L, 1));
	if(pMgr == nullptr)
	{
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("[net]SessionMgr_Send  param1 not USessionMgr",L);
		return 0;
	}	
	int32 nSid =  lua_type(L,2) != LUA_TNUMBER?lua_tointeger(L,2):0;	
	int32 msgId = lua_tointeger(L,3);
	Table* msg = (Table* )lua_topointer(L,4);
	pMgr->Send(nSid, msgId, msg, L, pMgr->GetLuaOverSend(), 2);
	return 0;
}

static const luaL_Reg USessionMgrLib[] =
{    
	{"ManagerBindLuaTable",SessionMgr_ManagerBindLuaTable},
	{"CallRpc",SessionMgr_CallRpc},
	{"CreateRpc", SessionMgr_CreateRpc},
	{"Send",SessionMgr_Send},
	{ nullptr, nullptr }
};

BEGIN_EXPORT_REFLECTED_CLASS(USessionMgr)
	ADD_LIB(USessionMgrLib)
	ADD_FUNCTION(SendBean)
	ADD_FUNCTION(RegisterMessage)
	ADD_FUNCTION(RegNotifyCallback)
	ADD_FUNCTION(RegSendOverride)
END_EXPORT_CLASS()
IMPLEMENT_EXPORTED_CLASS(USessionMgr)