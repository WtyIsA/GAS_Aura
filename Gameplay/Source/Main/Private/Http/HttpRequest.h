#pragma once

#include "IPlatformFilePak.h"
#include "Interfaces/IHttpRequest.h"
#include "Net/Marshal.h"
#include "Net/UnMarshal.h"

class USessionMgr;
typedef void (*HttpCallback)(USessionMgr* pMgr, int id,FString strUrl, FUnMarshal& obs, int nType, const void* pFun );

class FHttpClientRequest
{
public:
	FHttpClientRequest(USessionMgr* pMgr, bool bDelete =false);
	void HttpPost(FString strUrl,FMarshal& obs, int nType, HttpCallback pCallback,   int reposId, const void* pFun);
	void OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
private:
	int			m_nType;
	int		m_callbackId;
	FString		m_strUrl;
	const void*	m_pFun;
	HttpCallback m_pCallback;
	bool		m_bDeleteWhenComplete;
	USessionMgr*	m_pSessionMgr;
};
