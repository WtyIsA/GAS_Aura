#include "HttpRequest.h"

#include <vector>



#include "HttpDownloadMgr.h"
#include "HttpModule.h"
#include "LuaManger.h"
#include "Interfaces/IHttpResponse.h"
#include "Net/Marshal.h"
#include "Net/MessagePool.h"

FHttpClientRequest::FHttpClientRequest(USessionMgr* pMgr, bool bDelete)
{
	m_pSessionMgr = pMgr;
	m_bDeleteWhenComplete = bDelete;
}

void FHttpClientRequest::HttpPost(FString strUrl,FMarshal& obs, int nType, HttpCallback pCallback,   int reposId,const void* pFun)
{	
	m_callbackId = reposId;
	m_strUrl = strUrl;
	m_pCallback = pCallback;
	m_nType = nType;
	m_pFun = pFun;
	if(!CHttpDownloadMgr::CheckUrl(strUrl))
	{
		if(m_pCallback)
		{
			FUnMarshal *ibs = CMessagePool::Get()->GetUnMarshal();
			m_pCallback(m_pSessionMgr, m_callbackId, m_strUrl,*ibs, m_nType, m_pFun);
		}
		return;
	}
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request(FHttpModule::Get().CreateRequest());
	Request->SetHeader("Content-Type","text/javascript;charset=utf-8");
	Request->SetVerb("POST");
	Request->SetURL(m_strUrl);
	TArray<uint8> content;
	content.Append((uint8*)obs.GetData(),obs.Length());
	Request->SetContent(content);
	Request->OnProcessRequestComplete().BindRaw(this, &FHttpClientRequest::OnRequestComplete);
	Request->ProcessRequest();
}

void FHttpClientRequest::OnRequestComplete(FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bSucceeded)
{
	if (!bSucceeded || ResponsePtr->GetResponseCode() == 404)
	{
		UE_LOG(GAME,Warning,TEXT("[http]FHttpClientRequest::OnRequestComplete error %d "),ResponsePtr->GetResponseCode())
		if(m_pCallback)
		{
			FUnMarshal *ibs = CMessagePool::Get()->GetUnMarshal();
			m_pCallback(m_pSessionMgr,m_callbackId, m_strUrl,*ibs, m_nType, m_pFun);
		}
		if(m_bDeleteWhenComplete)
			delete this;
		return;
	}
	FUnMarshal *ibs = CMessagePool::Get()->GetUnMarshal(); // ( ResponsePtr->GetContent(), true);
	ibs->AttachArray(ResponsePtr->GetContent(), true);
	
	if(m_pCallback)
		m_pCallback(m_pSessionMgr,m_callbackId, m_strUrl,*ibs, m_nType, m_pFun);	

	if(m_bDeleteWhenComplete)
		delete this;
}