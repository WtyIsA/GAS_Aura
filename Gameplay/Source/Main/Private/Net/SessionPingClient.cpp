#include "SessionPingClient.h"
#include "LuaManger.h"
#include "SocketPrivateHeader.h"
#include "SocketSubsystem.h"
#include "Async/Async.h"
#include "Sdk/SdkInterface.h"

struct UdpPingDataInfo
{
	uint64  icmp_timestamp;//时间戳
	uint8 data[0];
};

USessionPingClient::USessionPingClient() :USession()
{
	memset(m_sendBuffer, 0, DATABUFFER_MAXLEN);
	memset(m_recvBuffer,0,DATABUFFER_MAXLEN);	
	m_sendLen=sizeof(UdpPingDataInfo);
}

bool USessionPingClient::InitSocket()
{
	ISocketSubsystem* SocketsSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	m_nowstate = SessionState_Connected;
	m_socket = SocketsSystem->CreateSocket(NAME_DGram,TEXT("SOCK_PING"), FNetworkProtocolTypes::IPv4);
	if(m_socket == nullptr)
		return false;
	m_socket->SetNonBlocking();
	m_socket->SetNoDelay(true);
	return true;
}


void USessionPingClient::BeginDestroy()
{
	Super::BeginDestroy();
	CleanPings();
}

void USessionPingClient::InitPingData()
{	
	UdpPingDataInfo *icmp_hdr =(struct UdpPingDataInfo *)(m_sendBuffer);  		
	icmp_hdr->icmp_timestamp = (uint64)(FPlatformTime::Seconds() * 1000);
}

void USessionPingClient::Ping(const FString& strIp, int port)
{
	if(!m_hostToAddr.Contains(strIp))
	{
		ISocketSubsystem* SocketsSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		TSharedPtr<FInternetAddr> Addr = SocketsSystem->GetAddressFromString(strIp);
		if (!Addr.IsValid())
		{
			USessionPingClient* pSession = this;
			auto AsyncResolverHandler = [pSession, port, strIp](FAddressInfoResult Results)
			{
				AsyncTask(ENamedThreads::GameThread, [pSession, port, strIp,Results]()
				{
					if (Results.ReturnCode != SE_NO_ERROR || Results.Results.Num() == 0)
					{
						UE_LOG(GAME, Error, TEXT("socket,getaddresinfo,error,%s"), *strIp);
						return;
					}
					TSharedRef<FInternetAddr> Addr2 = Results.Results[0].Address;
					Addr2->SetPort(port);
					pSession->m_Addrs.Add(Addr2);
					pSession->m_hostToAddr.Add(strIp, Addr2);
				});
			};
			SocketsSystem->GetAddressInfoAsync(AsyncResolverHandler, *strIp, *FString::Printf(TEXT("%d"), port),
				EAddressInfoFlags::AllResultsWithMapping | EAddressInfoFlags::OnlyUsableAddresses, NAME_None, ESocketType::SOCKTYPE_Datagram);
			return;
		}
		Addr->SetPort(port);
		m_Addrs.Add(Addr);
		m_hostToAddr.Add(strIp, Addr);
	}
	else
	{
		m_Addrs.Add(m_hostToAddr[strIp]);
	}
}

int32 USessionPingClient::GetPing(const FString& strIp)
{	
	if(!m_hostToPing.Contains(strIp))
		return -1;
	return m_hostToPing[strIp];
}

void USessionPingClient::CleanPings()
{
	m_Addrs.Empty();
	m_hostToAddr.Empty();
	m_hostToPing.Empty();
}


void USessionPingClient::Recv()
{
	ISocketSubsystem* SocketsSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	TSharedRef<FInternetAddr> Sender = SocketsSystem->CreateInternetAddr();	
	int32 nRecvLen = 0;
	m_socket->RecvFrom(m_recvBuffer, DATABUFFER_MAXLEN,nRecvLen,*Sender);
	if(nRecvLen > 0)
	{
		if(nRecvLen != m_sendLen)
		{
			UE_LOG(GAME,Warning,TEXT("[net]ping recv data len error,%d != %d"), nRecvLen,  m_sendLen);
			return;
		}

		UdpPingDataInfo *icmp = (struct UdpPingDataInfo *)(m_recvBuffer);
		SPingResult *result = new SPingResult( Sender,(uint32) ((uint64)(FPlatformTime::Seconds() * 1000) - icmp->icmp_timestamp));
		m_recvQueue.Enqueue(result);		
	}
}

void USessionPingClient::Tick()
{
	if(!SocketIsValid())
		return;
	if(m_Addrs.Num())
	{
		int nHadSend = 0;
		InitPingData();	
		for(int i = m_Addrs.Num() - 1; i >= 0;i--)
		{
			m_socket->SendTo(m_sendBuffer, m_sendLen,nHadSend,*m_Addrs[i]);
		}
		m_Addrs.Empty();
	}
	SPingResult* pingResult = nullptr;
	while (m_recvQueue.Dequeue(pingResult))
	{
		for(auto& item:m_hostToAddr)
		{
			if(item.Value->GetRawIp()== pingResult->addr->GetRawIp())
			{
				m_hostToPing.Add(item.Key, pingResult->useTime);
				break;
			}
		}
		delete pingResult;
	}
	
}
