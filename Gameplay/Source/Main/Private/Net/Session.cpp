#include "Session.h"


#include "LuaManger.h"
#include "IPAddress.h"
#include "SessionMgr.h"
#include "SocketSubsystem.h"

int	USession::m_sessionGlobleId = 1;

USession::USession()
{
	m_nowstate = m_oldstate = SessionState_init;
	m_sessionId = m_sessionGlobleId++;	
	m_socket = nullptr;
#ifndef  PLATFORM_HAS_BSD_SOCKETS	
	UE_LOG(GAME,Error,TEXT("socket,not define PLATFORM_HAS_BSD_SOCKETS"));
	check(false);	
#else
	
#endif

	m_pMgr = nullptr;
}


void USession::AttachSocket(FSocket* pSocket, SessionState state)
{
	if(m_socket != nullptr)
		Close();
	m_socket = pSocket;
	m_oldstate = m_nowstate = state;
}

void USession::BeginDestroy()
{
	Close();
	Super::BeginDestroy();
}

void USession::Close()
{
	if(m_socket == nullptr || m_nowstate == SessionState_Close)
		return;
	m_nowstate = SessionState_Close;
	m_socket->Shutdown(ESocketShutdownMode::ReadWrite);
	m_socket->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(m_socket);
	m_socket = nullptr;
	
	ClearData();
}



uint64 USession::GetSocket()
{
	if(m_socket == nullptr)
		return 0;
	return m_socket->GetSocket();
}

bool USession::SocketIsValid()
{
	if(m_socket == nullptr)
	{
		//UE_LOG(GAME, Warning, TEXT("USession::m_socket is null"));
		return false;
	}
	if(m_nowstate == SessionState_Connecting ||
		m_nowstate == SessionState_Connected ||
		m_nowstate == SessionState_Listen)
		return true;
	else
	{
		return false;
	}
}


void USession::Tick()
{
	
}


FString USession::GetIpStr(FInternetAddr& pAddr)
{
	TArray<uint8> nBytes = pAddr.GetRawIp();
	FString strIp;
	for(auto item:nBytes)
	{
		strIp.Appendf(TEXT("%d:"), item);
	}
	if(strIp.Len() > 1)
		strIp.RemoveAt(strIp.Len()-1);
	return strIp;
}
