#include "SelectInterrupter.h"

#include "LuaManger.h"
#include "SocketSubsystem.h"

CSelectInterrupter::CSelectInterrupter()
{
	m_readSocket = nullptr;
	m_wirteSocket = nullptr;
#ifndef  PLATFORM_HAS_BSD_SOCKETS	
	UE_LOG(GAME,Error,TEXT("[net]CSelectInterrupter socket,not define PLATFORM_HAS_BSD_SOCKETS"));
	check(false);	
#endif

}

CSelectInterrupter::~CSelectInterrupter()
{
	Reset();
}

void CSelectInterrupter::Reset()
{
	ISocketSubsystem* SocketsSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (m_wirteSocket)
	{
		SocketsSystem->DestroySocket(m_wirteSocket);
		m_wirteSocket = nullptr;
	}
	if (m_readSocket)
	{
		SocketsSystem->DestroySocket(m_readSocket);
		m_readSocket = nullptr;
	}
}


bool CSelectInterrupter::OpenInterrupter()
{
	Reset();
	ISocketSubsystem* SocketsSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	FString strIp(TEXT("127.0.0.1"));
	TSharedPtr<FInternetAddr> ipAddr = SocketsSystem->CreateInternetAddr(FNetworkProtocolTypes::IPv4);
	ipAddr->SetLoopbackAddress();
	FName ipvx = FNetworkProtocolTypes::IPv4;
	FSocket* listenSocket = SocketsSystem->CreateSocket(NAME_Stream,TEXT("default"), ipvx);
	check(listenSocket != nullptr);
	if(!listenSocket->Bind(*ipAddr))
	{
		UE_LOG(GAME, Error, TEXT("[net]socket,bind error,%s,%d"), *strIp,SocketsSystem->GetLastErrorCode());
		SocketsSystem->DestroySocket(listenSocket);
		return false;
	}	
	if(!listenSocket->Listen(1))
	{
		UE_LOG(GAME, Error, TEXT("[net]socket,listen error,%s,%d"), *strIp,SocketsSystem->GetLastErrorCode());
		SocketsSystem->DestroySocket(listenSocket);
		return false;
	}
	m_wirteSocket = SocketsSystem->CreateSocket(NAME_Stream,TEXT("default"), ipvx);
	check(m_wirteSocket);
	TSharedPtr<FInternetAddr> ipConAddr = SocketsSystem->CreateInternetAddr(FNetworkProtocolTypes::IPv4);
	ipConAddr->SetLoopbackAddress();
	ipConAddr->SetPlatformPort(listenSocket->GetPortNo());
	if(!m_wirteSocket->Connect(*ipConAddr))
	{
		UE_LOG(GAME, Error, TEXT("[net]CSelectInterrupter,Connect error,%d"),SocketsSystem->GetLastErrorCode());
		SocketsSystem->DestroySocket(listenSocket);
		SocketsSystem->DestroySocket(m_wirteSocket);
		m_wirteSocket = nullptr;
		return false;
	}
	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	m_readSocket = listenSocket->Accept(*RemoteAddress, TEXT("RemoteConnection"));
	if(m_readSocket == nullptr)
	{
		UE_LOG(GAME, Error, TEXT("[net]CSelectInterrupter,Accept error,%d"),SocketsSystem->GetLastErrorCode());
		SocketsSystem->DestroySocket(listenSocket);
		SocketsSystem->DestroySocket(m_wirteSocket);
		m_wirteSocket = nullptr;
		return false;
	}
	SocketsSystem->DestroySocket(listenSocket);
	UE_LOG(GAME,Display,TEXT("[net]OpenInterrupter sucess,%p，%p"), m_readSocket, m_wirteSocket);
	return true;
}

uint64 CSelectInterrupter::GetSocket()
{
	if(m_readSocket == nullptr)
		return 0;
	return m_readSocket->GetSocket();
}
#define InterrupterRecvDataBufferLen 100
void CSelectInterrupter::RecvData()
{
	if(m_readSocket == nullptr)
		return;
	uint8 temp[InterrupterRecvDataBufferLen] = {0};
	int nRecvLen = 0;
	if(!m_readSocket->Recv(temp, InterrupterRecvDataBufferLen, nRecvLen))
	{
		UE_LOG(GAME,Display,TEXT("[net]CSelectInterrupter is closed?"))
		OpenInterrupter();
		return;
	}
	while(nRecvLen == InterrupterRecvDataBufferLen)
	{
		m_readSocket->Recv(temp, InterrupterRecvDataBufferLen, nRecvLen);
	}
}

void CSelectInterrupter::Interrupt()
{
	if(m_wirteSocket)
	{
		int32 nSendByte = 0;
		m_wirteSocket->Send((uint8*)"\0",1, nSendByte);
	}
	else
	{
		UE_LOG(GAME,Display,TEXT("[net]CSelectInterrupter m_wirteSocket is null ,try open"))
		OpenInterrupter();
	}
}