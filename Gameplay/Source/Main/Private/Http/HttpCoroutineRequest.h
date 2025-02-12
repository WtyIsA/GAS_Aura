#pragma once

#include "IPlatformFilePak.h"
#include "Interfaces/IHttpRequest.h"

struct lua_State;
class FHttpCoroutineRequest
{
public:
	FHttpCoroutineRequest( bool bDelete =false);
	virtual ~FHttpCoroutineRequest(){}
	void OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	virtual bool Init(lua_State* L,  FString strUrl,  FString method,  FString content, TArray<FString> headers);
	virtual bool Process();
protected:
	bool	m_bDeleteWhenComplete;
	lua_State*	m_CorluaState;
	FString		m_url;
	FString		m_method;
	FString		m_content;
	TArray<FString> m_headers;
};
