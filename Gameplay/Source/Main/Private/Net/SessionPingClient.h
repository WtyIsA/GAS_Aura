#pragma once

#include "CoreMinimal.h"

#include "Session.h"
#include "UnMarshal.h"

#include "SessionPingClient.generated.h"

#define DATABUFFER_MAXLEN  200


UCLASS()
class USessionPingClient: public USession
{
	GENERATED_BODY()
public:
	USessionPingClient();

	UFUNCTION()
	void				Ping(const FString& strIp, int port);
	UFUNCTION()
	int32				GetPing(const FString& strIp);
	UFUNCTION()
	void				CleanPings();
	virtual void		BeginDestroy() override;
	virtual void		Recv();
	bool				InitSocket();
private:
	void				InitPingData();
	virtual void		Tick() override;
private:
	
	struct SPingResult
	{
		SPingResult(TSharedPtr<FInternetAddr> pAddr, uint32 time)
		{
			addr = pAddr;
			useTime = time;
		}
		SPingResult(SPingResult& other)
		{
			addr = other.addr;
			useTime = other.useTime;
		}
		TSharedPtr<FInternetAddr>	addr;
		uint32						useTime;
	};
	
	TQueue<SPingResult*>	m_recvQueue;
	uint16			m_sendLen;
	uint8			m_sendBuffer[DATABUFFER_MAXLEN];
	uint8			m_recvBuffer[DATABUFFER_MAXLEN];
	TArray<TSharedPtr<FInternetAddr>>	m_Addrs;
	TMap<FString,TSharedPtr<FInternetAddr>>		m_hostToAddr;
	TMap<FString, uint32>		m_hostToPing;

	
};
