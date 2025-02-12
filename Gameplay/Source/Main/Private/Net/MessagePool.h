#pragma once


class FMarshal;
class FUnMarshal;
class CMessagePool
{
	CMessagePool(int nMarshalCount=5,int nUnMarshalCount=5);
	~CMessagePool();
public:
	static  CMessagePool*	Get();
	static  void			Free();

	FMarshal*	GetMarshal();
	FUnMarshal*	GetUnMarshal(int bufferLen = 0);
	FUnMarshal*	GetUnMarshalThread(int bufferLen = 0);
	void		RecycleMessage(FMarshal* msg);
	void		RecycleMessage(FUnMarshal* msg);
private:
	TArray<FMarshal*>	m_MarshalPool;
	TArray<FUnMarshal*>	m_UnMarshalPool;
	TArray<FUnMarshal*>	m_UnMarshalPoolThread;
	static  CMessagePool*	m_handle;
};