#pragma once

#include "CoreMinimal.h"

#include "Marshal.h"
#include "Session.h"
#include "UnMarshal.h"

#include "SessionClient.generated.h"


UCLASS()
class USessionClient: public USession
{
	GENERATED_BODY()
public:
	USessionClient();

	UFUNCTION()
    void SetSocketBufferSize(int32 recvSize,int32 writeSize);	
	UFUNCTION()
    void SetAliveTimeout(int32  nTimeout);
	UFUNCTION()
    bool ReConnect(int32 connectTimeOut = 30);
	UFUNCTION()
	void ResetIpAndConnect(const FString& strIp, uint32 nPort,int32 connectTimeOut = 30);
	void SetForwardInfo(int forwardMsgId,const void* notifyLuaFun);
	
	virtual void	Send() override;
	virtual void	Send(FMarshal* Obs);
	virtual void	Recv() override;
	virtual void	SendMsg(int MsgId,void* Msg);
	void			Forward(int MsgId,void* Msg,  const FString& clientId);
	bool			Connect(const FString& Ip, uint32 port, int32 connectTimeOut = 30);
	
	virtual void	Tick() override;
	virtual bool	HasDataToSend() override{return !m_sendQueue.IsEmpty();}
	

	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FSeesionConnectEvent OnConnected;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FSeesionConnectEvent OnConnectFailed;
	UPROPERTY(EditAnywhere, Category=Events, meta=( IsBindableEvent="True" ))
	FSeesionConnectEvent OnDisConnected;
	
protected:
	virtual void	ClearData()override;
	bool			Send(FMarshal* msg, int32 & nHadSendSize);
	void			Recv(uint8* buffer,int nLen);
	void			GenMsg(uint8* buffer,int nLen);

protected:
	
	TQueue<FMarshal*>	m_sendQueue;
	TQueue<FUnMarshal*>	m_recvQueue;
	
	FString				m_host;
	uint32				m_port;
	TArray<uint8>		m_tempBuffer;
	uint32				m_aliveTimeout;
	uint32				m_connectTimeOut;
	uint64				m_lastAliveTime;

	uint32				m_forwardMsgId;
	const void*			m_forwardNotifyFun;
};
