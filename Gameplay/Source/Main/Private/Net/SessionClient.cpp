#include "SessionClient.h"

#include "LuaManger.h"
#include "IPAddress.h"
#include "lauxlib.h"
#include "Marshal.h"
#include "MemTracker.h"
#include "MessagePool.h"
#include "SessionMgr.h"
#include "SocketSubsystem.h"
#include "UnLuaBase.h"
#include "UnLuaEx.h"


USessionClient::USessionClient() :USession()
{
	m_lastAliveTime =  (uint64)FPlatformTime::Seconds();
	m_connectTimeOut = 30;
	m_aliveTimeout = 0;
	m_port = 0;
	m_forwardMsgId = 0;
	m_forwardNotifyFun = nullptr;
}

bool USessionClient::Connect(const FString& strIp, uint32 port, int32 connectTimeOut)
{
	if(connectTimeOut <= 0)
		connectTimeOut = 30;
	m_host = strIp;
	m_port = port;
	ISocketSubsystem* SocketsSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	FString str;
	TSharedPtr<FInternetAddr> ipAddr = SocketsSystem->GetAddressFromString(strIp);
	FName ipvx = FNetworkProtocolTypes::IPv4;
	if (!ipAddr.IsValid())
	{
		FAddressInfoResult ipresutlt = SocketsSystem->GetAddressInfo(*strIp,nullptr, EAddressInfoFlags::Default, NAME_Stream,ESocketType::SOCKTYPE_Streaming);
		if (ipresutlt.ReturnCode != SE_NO_ERROR || ipresutlt.Results.Num() == 0)
		{
			UE_LOG(GAME, Error, TEXT("socket,getaddresinfo,error,%s"), *strIp);
			OnConnectFailed.ExecuteIfBound(this, m_sessionId);
			return false;
		}
		for(auto& item : ipresutlt.Results)
		{
			if(item.AddressProtocol == ESocketProtocolFamily::IPv6)
				ipvx = item.AddressProtocolName;
			UE_LOG(GAME, Display, TEXT("[net],getaddresinfo,%s address info=%s,ipvx=%s"), *strIp,*item.Address->ToString(true),*item.AddressProtocolName.ToString());
		}
		ipAddr = ipresutlt.Results[0].Address;
		ipvx = ipresutlt.Results[0].AddressProtocolName;		
	}
	ipAddr->SetPort(port);
	m_socket = SocketsSystem->CreateSocket(NAME_Stream,TEXT("default"), ipvx);
	if(m_socket == nullptr)
	{
		UE_LOG(GAME, Error, TEXT("[net]SocketsSystem->CreateSocket,error,%s,code=%d"), *strIp,SocketsSystem->GetLastErrorCode());
		return false;
	}	
	m_socket->SetNonBlocking();
	m_socket->SetNoDelay(true);
	if(!m_socket->Connect(*ipAddr))
	{
		UE_LOG(GAME, Error, TEXT("socket,Connect,error,%s,code=%d"), *strIp,SocketsSystem->GetLastErrorCode());
		OnConnectFailed.ExecuteIfBound(this, m_sessionId);
		return false;
	}
	m_lastAliveTime = (uint64)FPlatformTime::Seconds();	
	m_oldstate = m_nowstate = SessionState_Connecting;
	m_connectTimeOut = connectTimeOut;
	return true;
}

void USessionClient::ResetIpAndConnect(const FString& strIp, uint32 nPort, int32 connectTimeOut)
{	
	m_host = strIp;
	m_port = nPort;
	if(connectTimeOut >= 0)
		ReConnect(connectTimeOut);
}

void USessionClient::SetForwardInfo(int forwardMsgId, const void* notifyLuaFun)
{
	m_forwardMsgId = forwardMsgId;
	m_forwardNotifyFun = notifyLuaFun;
}


bool USessionClient::ReConnect(int32 connectTimeOut)
{
	if(m_socket != nullptr)
	{
		Close();
		m_socket = nullptr;
	}
	m_oldstate = m_nowstate = SessionState_init;
	return  Connect(m_host, m_port, connectTimeOut);
}


void USessionClient::SetSocketBufferSize(int32 recvSize, int32 writeSize)
{
	if(m_socket == nullptr)
		return;
	int32 nSize = 0;
	m_socket->SetReceiveBufferSize(recvSize, nSize);
	m_socket->SetSendBufferSize(writeSize, nSize);
}

void USessionClient::SetAliveTimeout(int32  nTimeout)
{
	m_aliveTimeout = nTimeout;
}


void USessionClient::Tick()
{
	check(m_pMgr);
	if(m_recvQueue.IsEmpty())
	{
		if(m_nowstate == SessionState_Connected && m_aliveTimeout > 0)
		{
			uint64 nowTime = (uint64)FPlatformTime::Seconds();
			if(nowTime - m_lastAliveTime >= m_aliveTimeout)
			{
				UE_LOG(GAME,Display,TEXT("[net]session tick alive timeout,please check net"))
				Close();
				OnDisConnected.ExecuteIfBound(this, m_sessionId);
			}
		}
	}
	else
	{
		FUnMarshal* msg = nullptr;
		if(m_forwardMsgId > 0)
		{
			while (m_recvQueue.Dequeue(msg))
			{
				m_pMgr->RecvMsg(m_sessionId, msg, m_forwardMsgId, m_forwardNotifyFun);
				CMessagePool::Get()->RecycleMessage(msg);
			}
		}
		else
		{
			while (m_recvQueue.Dequeue(msg))
			{
				m_pMgr->RecvMsg(m_sessionId, msg);
				CMessagePool::Get()->RecycleMessage(msg);
			}
		}
	}
	if(m_oldstate != m_nowstate)
	{
		UE_LOG(GAME,Display,TEXT("[net]%d,connect state change %d->%d"), m_sessionId, m_oldstate,m_nowstate);	
		if(m_nowstate == SessionState_Connected)
		{
			OnConnected.ExecuteIfBound(this,m_sessionId);
		}
		else if(m_nowstate == SessionState_ConnectFailed)
		{
			m_nowstate = SessionState_init;
			OnConnectFailed.ExecuteIfBound(this, m_sessionId);
		}
		else if(m_nowstate == SessionState_DisConnect)
			OnDisConnected.ExecuteIfBound(this,m_sessionId);
		m_oldstate = m_nowstate;
	}
	else
	{
		if(m_nowstate == SessionState_Connecting && m_connectTimeOut > 0)
		{
			uint64 nowTime = (uint64)FPlatformTime::Seconds();
			if(nowTime - m_lastAliveTime >= m_connectTimeOut)
			{
				Close();
				OnConnectFailed.ExecuteIfBound(this, m_sessionId);
			}
		}
	}	
}

void USessionClient::ClearData()
{	
	m_tempBuffer.Empty();
	FMarshal* toSend = nullptr;
	while (m_sendQueue.Dequeue(toSend))
	{
		CMessagePool::Get()->RecycleMessage(toSend);
	}
	FUnMarshal* toRecv = nullptr;
	while (m_recvQueue.Dequeue(toRecv))
	{
		CMessagePool::Get()->RecycleMessage(toRecv);
	}
}

void USessionClient::Send(FMarshal* Obs)
{
	if(Obs)
	{
		m_sendQueue.Enqueue(Obs);
		if(m_pMgr)
			m_pMgr->InterruptSelect();
	}
}


void USessionClient::SendMsg(int MsgId, void* Msg)
{	
    check(m_pMgr)
	Table* TablePtr = (Table*)Msg;    
	if(TablePtr != nullptr)
	{
		FMarshal* Obs = CMessagePool::Get()->GetMarshal();
		Obs->Write(MsgId);
		if(!m_pMgr->Marshal(TablePtr, Obs, MsgId))
		{
			CMessagePool::Get()->RecycleMessage(Obs);
			UE_LOG(GAME, Error,TEXT("[net]Marshal,error,msgid=%d"),MsgId);
			return;
		}
		Obs->Pack();
		if(USessionMgr::m_bShowMessageLog)
		{
			SLuaMsgInfo* pInfo = m_pMgr->GetMsgInfo(MsgId);
			UE_LOG(GAME, Display, TEXT("[msg]send msg id=%d, len=%d, name=%s"), MsgId, Obs->Length(), ANSI_TO_TCHAR(pInfo->m_msgName.c_str()));
		}
		m_sendQueue.Enqueue(Obs);
		if(m_pMgr)
			m_pMgr->InterruptSelect();
	}
}

void USessionClient::Forward(int MsgId, void* Msg,const FString& clientId)
{
	check(m_pMgr);
	Table* TablePtr = (Table*)Msg;    
	if(TablePtr != nullptr)
	{
		FMarshal* Obs = CMessagePool::Get()->GetMarshal();
		Obs->Write(MsgId);
		if(!m_pMgr || !m_pMgr->Marshal(TablePtr, Obs, MsgId))
		{
			CMessagePool::Get()->RecycleMessage(Obs);
			UE_LOG(GAME, Error,TEXT("[net]Marshal,error,msgid=%d"),MsgId);
			return;
		}
		Obs->Pack();
		if(USessionMgr::m_bShowMessageLog)
		{
			SLuaMsgInfo* pInfo = m_pMgr->GetMsgInfo(MsgId);
			UE_LOG(GAME, Display, TEXT("[msg]send msg id=%d, len=%d, name=%s"), MsgId, Obs->Length(), ANSI_TO_TCHAR(pInfo->m_msgName.c_str()));
		}
		FMarshal* ForwardObs = CMessagePool::Get()->GetMarshal();
		ForwardObs->Write(m_forwardMsgId);
		ForwardObs->WriteStr(clientId);
		ForwardObs->WriteOctets(Obs);
		ForwardObs->Pack();
		CMessagePool::Get()->RecycleMessage(Obs);
		m_sendQueue.Enqueue(ForwardObs);
		if(m_pMgr)
			m_pMgr->InterruptSelect();
	}
}


bool USessionClient::Send(FMarshal* msg, int32 & nHadSendSize)
{
	if(msg == nullptr)
		return true;
	check(m_socket);
	int nHadSend = 0;
	if(!m_socket->Send(msg->GetData(), msg->Length(), nHadSend))
		return false;
	nHadSendSize += nHadSend;
	if(nHadSend < msg->Length())
	{
		msg->RemoveLen(nHadSend);
		return false;
	}
	return true;
}

void USessionClient::Send()
{
	check(m_socket);
	FMarshal* toSend = nullptr;
	int32 nHadSendSize = 0;
	while (m_sendQueue.Peek(toSend))
	{
		if(!Send(toSend, nHadSendSize))
			break;
		m_sendQueue.Pop();
		CMessagePool::Get()->RecycleMessage(toSend);
	}

}
#define RecvDataBufferLen 1024
void USessionClient::Recv()
{
	if(m_socket == nullptr)
		return;
	if(m_nowstate == SessionState_Close)
	{
		return;
	}
	MemTrackerScope(UnLua::ELuaLLMTag::MsgTag);
	uint8 temp[RecvDataBufferLen] = {0};
	int nRecvLen = 0;
	if(!m_socket->Recv(temp, RecvDataBufferLen, nRecvLen))
	{
		Close();
		SetConnectState(SessionState_DisConnect);
		return;
	}

	Recv(temp, nRecvLen);
	while(nRecvLen == RecvDataBufferLen)
	{
		m_socket->Recv(temp, RecvDataBufferLen, nRecvLen);
		Recv(temp, nRecvLen);
	}
	m_lastAliveTime = (uint64)FPlatformTime::Seconds();
}

void USessionClient::Recv(uint8* buffer,int nLen)
{
	if(nLen <= 0)
		return;
	//消息长度小于四个字节
	if(m_tempBuffer.Num() + nLen < 4)
	{
		m_tempBuffer.Append(buffer,nLen);
		return;
	}
	//缓冲区里没有，直接核对长度，生成消息，多余放入缓存
	if(m_tempBuffer.Num() == 0)
	{
		GenMsg(buffer,nLen);
	}
	else
	{
		int nTempBufferLen = m_tempBuffer.Num();
		//缓存长度大于四
		if(nTempBufferLen >= 4)
		{
			int32 nMsgLen = *(int32*)m_tempBuffer.GetData();
			nMsgLen = FEndianOpt::NetworkToHost(nMsgLen);
			if(nMsgLen <= m_tempBuffer.Num() + nLen)
			{
				int nNeedLen = nMsgLen - m_tempBuffer.Num();
				FUnMarshal* pMsg =  CMessagePool::Get()->GetUnMarshalThread(nMsgLen);// new FUnMarshal(m_tempBuffer, true);
				m_tempBuffer.Append(buffer, nNeedLen);
				pMsg->AttachArray(m_tempBuffer, true, m_tempBuffer.Num());
				m_tempBuffer.Empty();
				m_recvQueue.Enqueue(pMsg);
				nLen -= nNeedLen;
				buffer += nNeedLen;
				GenMsg(buffer, nLen);
			}
			else
			{
				m_tempBuffer.Append(buffer, nLen);
			}
		}
		else
		{
			//小于四个，直接补充四个长度以获取消息长度
			int nNeedLen = 4 - nTempBufferLen;
			m_tempBuffer.Append(buffer, nNeedLen);
			buffer += nNeedLen;
			nLen -= nNeedLen;
			Recv(buffer, nLen);
		}
	}
}

void USessionClient::GenMsg(uint8* buffer,int nLen)
{
	if(nLen < 4)
	{
		m_tempBuffer.Append(buffer, nLen);
		return;
	}
	while (true)
	{
		int32 nMsgLen = *(int32*)buffer;
		nMsgLen = FEndianOpt::NetworkToHost(nMsgLen);
		if(nMsgLen <= nLen)
		{
			FUnMarshal* pMsg =  CMessagePool::Get()->GetUnMarshalThread(nMsgLen);// new FUnMarshal(buffer, nMsgLen);
			pMsg->PushBuffer(buffer, nMsgLen);
			m_recvQueue.Enqueue(pMsg);
			buffer += nMsgLen;
			nLen -= nMsgLen;
			if(nLen < 4)
			{
				m_tempBuffer.Append(buffer, nLen);
				break;
			}
		}
		else
		{
			m_tempBuffer.Append(buffer, nLen);
			break;
		}
	}
}



BEGIN_EXPORT_REFLECTED_CLASS(USessionClient)
    ADD_NAMED_FUNCTION("Send", SendMsg)
	ADD_NAMED_FUNCTION("Forward", Forward)
	ADD_NAMED_FUNCTION("SetForwardInfo",SetForwardInfo)
END_EXPORT_CLASS()
IMPLEMENT_EXPORTED_CLASS(USessionClient)