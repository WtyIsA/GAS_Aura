#pragma once
#include "HttpCoroutineRequest.h"

class FHttpTranslateTextRequest : public FHttpCoroutineRequest
{
public:
	FHttpTranslateTextRequest( bool bDelete =false);
	virtual bool Init(lua_State* L,  FString strUrl,  FString method,  FString content, TArray<FString> headers);
	
};
