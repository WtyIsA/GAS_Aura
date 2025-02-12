#pragma once

#include "CoreMinimal.h"
#include "Session.h"

#include "SessionServer.generated.h"


UCLASS()
class USessionServer: public USession
{
	GENERATED_BODY()
public:
	USessionServer();

	bool			Listen(const FString& Ip, uint32 port, uint32 nMaxCount, uint32 connectedAliveTimeout);
	
	virtual void	Tick() override;
	virtual void	Recv() override;
	
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FSeesionConnectEvent OnRecvConnected;

private:
	uint32		m_connectedAliveTimeout;
};
