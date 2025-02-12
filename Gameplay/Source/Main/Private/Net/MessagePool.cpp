#include "MessagePool.h"

#include "LuaManger.h"
#include "Marshal.h"
#include "UnMarshal.h"

CMessagePool*	CMessagePool::m_handle = nullptr;

CMessagePool::CMessagePool(int nMarshalCount,int nUnMarshalCount)
{
	for(int i =0;i < nMarshalCount;i++)
	{
		FMarshal* pMsg = new FMarshal();
		m_MarshalPool.Add(pMsg);
	}
	for(int i =0;i < nUnMarshalCount;i++)
	{
		FUnMarshal* pMsg = new FUnMarshal();
		m_UnMarshalPool.Add(pMsg);
	}
	for(int i =0;i < nUnMarshalCount;i++)
	{
		FUnMarshal* pMsg = new FUnMarshal();
		m_UnMarshalPoolThread.Add(pMsg);
	}
}

CMessagePool::~CMessagePool()
{
	for(auto item: m_MarshalPool)
		delete item;
	m_MarshalPool.Empty();
}

CMessagePool* CMessagePool::Get()
{
	if(m_handle == nullptr)
		m_handle = new CMessagePool(5,50);
	return m_handle;	
}

void CMessagePool::Free()
{
	if(m_handle)
		delete m_handle;
	m_handle = nullptr;
}


FMarshal* CMessagePool::GetMarshal()
{
	for(auto item :m_MarshalPool)
	{
		if(!item->IsUsing())
		{
			item->SetUseing(true);
			item->ResetPos();
			return item;
		}
	}
	FMarshal* pMsg = new FMarshal(true);
	m_MarshalPool.Add(pMsg);
	return pMsg;
}

void CMessagePool::RecycleMessage(FMarshal* msg)
{
	if(msg)
		msg->SetUseing(false);
}

FUnMarshal* CMessagePool::GetUnMarshal(int bufferLen)
{
	for(auto item :m_UnMarshalPool)
	{
		if(!item->IsUsing())
		{
			if(bufferLen > 0)
			{
				if(item->GetArrayLen() >= bufferLen && item->GetArrayLen() < bufferLen*2)
				{
					item->SetUseing(true);
					item->ResetPos();
					return item;
				}
			}
			else
			{
				item->SetUseing(true);
				item->ResetPos();
				return item;
			}
		}		
	}
	if(bufferLen > 0)
	{
		for(auto item :m_UnMarshalPool)
		{
			if(!item->IsUsing())
			{
				item->SetUseing(true);
				item->ResetPos();
				return item;
			}
		}
	}
	FUnMarshal* pMsg = new FUnMarshal();
	pMsg->SetUseing(true);
	m_UnMarshalPool.Add(pMsg);
	return pMsg;
}

FUnMarshal* CMessagePool::GetUnMarshalThread(int bufferLen)
{
	for(auto item :m_UnMarshalPoolThread)
	{
		if(!item->IsUsing())
		{
			if(bufferLen > 0)
			{
				if(item->GetArrayLen() >= bufferLen && item->GetArrayLen() < bufferLen*2)
				{
					item->SetUseing(true);
					item->ResetPos();
					return item;
				}
			}
			else
			{
				item->SetUseing(true);
				item->ResetPos();
				return item;
			}
		}		
	}
	if(bufferLen > 0)
	{
		for(auto item :m_UnMarshalPoolThread)
		{
			if(!item->IsUsing())
			{
				item->SetUseing(true);
				item->ResetPos();
				return item;
			}
		}
	}
	FUnMarshal* pMsg = new FUnMarshal();
	pMsg->SetUseing(true);
	m_UnMarshalPoolThread.Add(pMsg);
	return pMsg;
}

void CMessagePool::RecycleMessage(FUnMarshal* msg)
{
	if(msg)
		msg->SetUseing(false);
}
