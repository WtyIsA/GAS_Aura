#pragma once

#include "CoreMinimal.h"

#include "SelectInterrupter.h"
#include "Session.h"
#include "SocketPrivateHeader.h"
#include "UnLuaInterface.h"
#include "msg/LuaRpcMsgInfo.h"
#include "SessionMgr.generated.h"

struct SLuaMsgInfo;
struct lua_State;
struct Table;
class FUnMarshal;
class FMarshal;

UCLASS()
class USessionMgr:public UObject, public FRunnable, public  FSingleThreadRunnable//, public IUnLuaInterface
{
	GENERATED_BODY()	
public:
	USessionMgr();
	virtual FString GetModuleName_Implementation() const;
	virtual void BeginDestroy() override;
	
	UFUNCTION()
	void Start();
	UFUNCTION()
	void Stop();
	UFUNCTION()
	virtual void Tick();
	UFUNCTION()
	void AddSession(USession* pSession);
	UFUNCTION()
    void RemoveSession(USession* pSession);
	UFUNCTION()
	UObject* CreateSession(const FString& strHost, uint32 port, int32 connectTimeOut = 30);
	UFUNCTION()
	UObject*  CreatePingSession(const FString& strIp, int port);
	UFUNCTION()
	UObject* Listen(const FString& strHost, uint32 port, uint32 nMaxCount, uint32 connectedAliveTimeout=0);

	void			InterruptSelect();
	USession*		GetSessionById(int32 sid);

	void			SendBean(FString url,int nType, FString msgName, void* msg, FString recvBeanName, const void* pFun);
	Table*			RecvBean(int sid,FUnMarshal& ibs);
	void			RecvMsg(int64 sid,FUnMarshal* ibs);
	void			RecvMsg(int64 sid,FUnMarshal* ibs, uint32 forwardId, const void* notifyFun);
	Table*			CreateTable(int msgId,int nArraySize = 1, int nHashSize = 2, bool bPushToTop = false);
	bool			UnMarshal(int64 sid, FUnMarshal* ibs, Table* table, int msgId);
	bool			Marshal(Table* table,FMarshal* obs, int msgId);
	SLuaMsgInfo*	GetMsgInfo(int msgId);
	
	void			Send(int32 nSessionId, int32 msgId, Table* msg);
	bool			RegisterMessage(void* msgTable);
	void			BindLuaTable(Table* pTable){m_bindLuaTable = pTable;}
	void			RegNotifyCallback(const void* pFun){m_pNotifyFun = pFun;}
	void			RegSendOverride(const void* pFun){m_pSendOverrideFun = pFun;}
	void			CallRpc(lua_State *L, Table* pTable, int32 MsgId, int otherSidIndex);
	Table*			CreateRpc(lua_State *L);
	SLRpcInfo*		FindRpc(int32 rpcId);
	
	
	static bool		m_bShowMessageLog;
	static bool		Notify(Table* pTable, int nType, const void*luaFun );
	Table*			NewRpcTable(lua_State *L,SLuaMsgInfo*  pMsg );
	const void*		GetLuaOverSend(){return m_pSendOverrideFun;}
	void			Send(int32 nSessionId, int32 msgId, Table* msg, lua_State *L, const void* luaFun, int otherSidIndex);

	bool			Marshal(int32 msgId, Table* msg, TArray<uint8>& msgData);
	Table*			UnMarshal(int64 sId, const TArray<uint8>& msgData);
protected:
	virtual uint32	Run() override;
	bool			Update();
	bool			CheckSessionInSelectList(const USession* pSession,const TArray<USession*>& sessionList );

	
	void			Notify(int64 sid,  Table* pTable, int msgId);
	void			Notify(const char* clientId,  Table* pTable, int msgId, const void* luaFun );
	void			FillMessageHead(Table* pTable, int64 sid, int nMsgType);
	int				GetMsgIdByName(FString& msgName);
	void			ProcessRecvMsg(int64 sid,FUnMarshal* ibs, int32 msgId, int32 nLength , const char* clientId,const void* luaFun);
	
	virtual  FSingleThreadRunnable* GetSingleThreadInterface( ){return this;}
private:
	TQueue<USession*>		m_addSessions;
	TQueue<USession*>		m_removeSessions;
	TArray<USession*>		m_sessions;
	TArray<USession*>		m_netThreadSessionList;
	bool					m_stopping;
	FRunnableThread*		m_thread;
	FTimespan				m_waitTime;
	fd_set					m_writeset;
	fd_set					m_readset;
	uint64					m_maxfd;
	CSelectInterrupter*		m_selectInterrupter;
	TMap<int, SLuaMsgInfo*> m_mapLuaMsgs;

	Table*					m_bindLuaTable;
	const void*				m_pNotifyFun;
	const void*				m_pSendOverrideFun;
	TMap<int32,SLRpcInfo>	m_rpcList;
	static int32			m_rpcId;
	bool					m_bCreateRealThread;
};
