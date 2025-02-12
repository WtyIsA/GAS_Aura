#include "HttpCoroutineRequest.h"

#include "HttpDownloadMgr.h"
#include "HttpModule.h"
#include "LuaManger.h"
#include "LuaMessageTableOptInC.h"
#include "Interfaces/IHttpResponse.h"
#include "Net/Marshal.h"
#include "Net/MessagePool.h"
#include "UnLuaEx.h"
#include "Windows/WindowsPlatformHttp.h"

FHttpCoroutineRequest::FHttpCoroutineRequest(bool bDelete)
{	
	m_bDeleteWhenComplete = bDelete;
	m_CorluaState = nullptr;
}

bool FHttpCoroutineRequest::Init(lua_State* L,  FString strUrl,  FString method,  FString content, TArray<FString> headers)
{
	if(!CHttpDownloadMgr::CheckUrl(strUrl))
		return false;
	if(method != TEXT("GET") && method != TEXT("POST"))
	{
		UE_LOG(GAME,Error,TEXT("[http],HttpCoroutineRequest,method=%s error,url=%s"), *method, *strUrl);
		return false;
	}
	if(UnLua::GetState() == L)
	{
		UE_LOG(GAME,Error,TEXT("[http],HttpCoroutineRequest not in Coroutine,url=%s"),  *strUrl);
		return false;
	}
	m_CorluaState = L;
	m_url = strUrl;
	m_content = content;
	m_headers = headers;
	m_method = method;
	

	return true;
}

bool FHttpCoroutineRequest::Process()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request(FHttpModule::Get().CreateRequest());
	
	if(m_headers.Num() > 1)
	{
		for(int i = 0; i < m_headers.Num() - 1; i += 2)
		{
			Request->SetHeader(m_headers[i], m_headers[i+1]);
		}
		if(!m_headers.Contains(TEXT("Content-Type")))
		{
			Request->SetHeader(TEXT("Content-Type"),TEXT("text/javascript;charset=utf-8"));
		}
	}
	else
	{
		Request->SetHeader(TEXT("Content-Type"),TEXT("text/javascript;charset=utf-8"));
	}
	Request->SetVerb(m_method);
	Request->SetURL(m_url);
	Request->SetContentAsString(m_content);
	Request->OnProcessRequestComplete().BindRaw(this, &FHttpCoroutineRequest::OnRequestComplete);
	Request->ProcessRequest();
	
	return true;
}


void FHttpCoroutineRequest::OnRequestComplete(FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bSucceeded)
{
	int NResults = 0;
	if (!bSucceeded || ResponsePtr->GetResponseCode() == 404)
	{
		UE_LOG(GAME,Error,TEXT("[http] HttpCoroutineRequest %s,responsecode=%d,content=%s"), *m_url, ResponsePtr->GetResponseCode(),*ResponsePtr->GetContentAsString());
		lua_pushnil(m_CorluaState);	
	}
	else
	{
		const TArray<uint8>& content = ResponsePtr->GetContent();
		lua_pushlstring(m_CorluaState, (const char*)content.GetData(), content.Num());
	}
	lua_resume(m_CorluaState,UnLua::GetState(),  1, &NResults);

	if(m_bDeleteWhenComplete)
		delete this;
}