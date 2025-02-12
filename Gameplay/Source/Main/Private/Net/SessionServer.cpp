#include "SessionServer.h"

#include "LuaManger.h"
#include "Session.h"
#include "SessionClient.h"
#include "SessionMgr.h"
#include "SocketSubsystem.h"
#include "Async/Async.h"

USessionServer::USessionServer() :USession()
{
	m_connectedAliveTimeout = 0;
}

void USessionServer::Tick()
{
	
}

bool USessionServer::Listen(const FString& Ip, uint32 port, uint32 nMaxCount, uint32 connectedAliveTimeout)
{
	ISocketSubsystem* SocketsSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	FString str;
	TSharedPtr<FInternetAddr> ipAddr = SocketsSystem->GetAddressFromString(Ip);
	FName ipvx = FNetworkProtocolTypes::IPv4;
	if (!ipAddr.IsValid())
	{
		UE_LOG(GAME, Error, TEXT("[net]socket,Listen,ip error,%s"), *Ip);
		return false;	
	}
	if(port > 0)
		ipAddr->SetPort(port);
	m_socket = SocketsSystem->CreateSocket(NAME_Stream,TEXT("default"), ipvx);
	check(m_socket != nullptr);
	m_socket->SetNonBlocking();
	m_socket->SetNoDelay(true);
	m_socket->SetReuseAddr();
	if(!m_socket->Bind(*ipAddr))
	{
		UE_LOG(GAME, Error, TEXT("[net]socket,bind error,%s,%d"), *Ip,SocketsSystem->GetLastErrorCode());
		return false;
	}
	if(nMaxCount == 0)
		nMaxCount = 1;
	if(!m_socket->Listen(nMaxCount))
	{
		UE_LOG(GAME, Error, TEXT("[net]socket,listen error,%s,%d"), *Ip,SocketsSystem->GetLastErrorCode());
		return false;
	}
	int32 nListenPort = m_socket->GetPortNo();
	UE_LOG(GAME,Display,TEXT("[net]listen,success,ip=%s:%d"), *Ip, nListenPort);
	m_oldstate = m_nowstate = SessionState_Listen;
	m_connectedAliveTimeout = connectedAliveTimeout;
	return true;
}


void USessionServer::Recv()
{
	check(m_socket);
	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	FSocket* ConnectionSocket = nullptr;
	do
	{
		ConnectionSocket = m_socket->Accept(*RemoteAddress, TEXT("RemoteConnection"));
		if (ConnectionSocket != nullptr)
		{
			USessionClient* pRecvClient = NewObject<USessionClient>();
			UE_LOG(GAME, Display, TEXT("[net]USessionServer,recvconnected,ip=%s:%d"), *GetIpStr(*RemoteAddress),RemoteAddress->GetPort());
			pRecvClient->AttachSocket(ConnectionSocket, SessionState_Connected);
			pRecvClient->SetAliveTimeout(m_connectedAliveTimeout);
			if (m_pMgr)
				m_pMgr->AddSession(pRecvClient);
			AsyncTask(ENamedThreads::GameThread, [this,pRecvClient ]()
			{
				this->OnRecvConnected.ExecuteIfBound(pRecvClient, pRecvClient->GetSessionId());
			});
		}
		else
		{
			break;
		}
	}
	while (true);
	
}
