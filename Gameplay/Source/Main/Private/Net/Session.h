#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "Session.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FSeesionConnectEvent, UObject*, obj, int32, param);


enum  SessionState 
{
	SessionState_init =0,
	SessionState_Listen,
	SessionState_Connecting,
	SessionState_Connected,
	SessionState_ConnectFailed,
	SessionState_DisConnect,
	SessionState_Close,
	SessionState_Remove,
};

class USessionMgr;
class FMarshal;
UCLASS()
class USession: public UObject
{
	GENERATED_BODY()
public:
	USession();
	virtual void BeginDestroy() override;
	UFUNCTION()
	void Close();		

	virtual void		Send(){};
	virtual void		Send(FMarshal* Obs){};
	virtual void		SendMsg(int MsgId,void* Msg){};
	virtual void		Recv(){};
	virtual void		Tick();
	
	void				AttachSocket(FSocket* pSocket, SessionState state);
    void				SetManager(USessionMgr* pMgr){m_pMgr = pMgr;}
	const USessionMgr*	GetManager() const{return m_pMgr;}
	uint64				GetSocket();
	SessionState		GetConnectState() const {return  m_nowstate;}
	void				SetConnectState(SessionState nowState){m_nowstate = nowState;}
	virtual bool		HasDataToSend(){return false;}
	virtual bool		SocketIsValid();
	int32				GetSessionId()const{return  m_sessionId;}
	static FString		GetIpStr(FInternetAddr& pAddr);
protected:
	virtual void		ClearData(){};
protected:
	static int			m_sessionGlobleId;
	FSocket*			m_socket;
	int					m_sessionId;	
	SessionState		m_nowstate;
	SessionState		m_oldstate;
	USessionMgr*		m_pMgr;
};
