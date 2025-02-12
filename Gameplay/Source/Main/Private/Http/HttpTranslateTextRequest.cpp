#include "HttpTranslateTextRequest.h"

#include "HttpDownloadMgr.h"
#include "LuaManger.h"
#include "Sdk/utils.h"

#define TRANSLATEACCESSKEYID  TEXT("AKAPYmYwMzUyZjlmNDE2NDI1NzhhMmE5NWJmODJjZDk2YzQ")
#define TRANSLATESECRET TEXT("WWpFd1kyUTVPVEptTURjNU5HWXhaR0l5TW1Fd05USTNOV1l4TWpBNE5qSQ==")

FHttpTranslateTextRequest::FHttpTranslateTextRequest(bool bDelete) : FHttpCoroutineRequest(bDelete)
{
	
}

bool FHttpTranslateTextRequest::Init(lua_State* L, FString strUrl, FString method, FString content, TArray<FString> headers)
{
	if(!FHttpCoroutineRequest::Init(L, strUrl, method, content, headers))
		return false;
	FString strHost, strURIPath, strQueryString;
	if(!CHttpDownloadMgr::ParseUrl(strUrl,strHost, strURIPath, strQueryString))
	{
		UE_LOG(GAME,Error,TEXT("[http]FHttpTranslateTextRequest url error %s"), *strUrl);
		return false;
	}
	
	FDateTime nowTime = FDateTime::UtcNow();
	int32 OutYear,  OutMonth, OutDay;
	nowTime.GetDate(OutYear,OutMonth,OutDay);
	
	m_headers.Add(TEXT("X-Date"));
	FString xdate = FString::Printf(TEXT("%d%02d%02dT%02d%02d%02dZ"),OutYear,OutMonth,OutDay, nowTime.GetHour(),nowTime.GetMinute(),nowTime.GetSecond());
	m_headers.Add(xdate);
	
	if(!m_headers.Contains(TEXT("Content-Type")))
	{
		m_headers.Add(TEXT("Content-Type"));
		m_headers.Add(TEXT("application/json"));
	}
	FString contentSha256 = rtc::tools::Sha256(content);
	m_headers.Add(TEXT("X-Content-Sha256"));
	m_headers.Add(contentSha256);
	FString xShortData = FString::Printf(TEXT("%d%02d%02d"),OutYear,OutMonth,OutDay);
	FString strRegion(TEXT("ap-singapore-1"));
	FString strServer(TEXT("translate"));
	FString CanonicalHeaders = FString::Printf(TEXT("x-content-sha256:%s\nx-date:%s\n"),*contentSha256, *xdate);
	FString SignedHeaders = TEXT("x-content-sha256;x-date");
	FString CanonicalRequest = FString::Printf(TEXT("%s\n%s\n%s\n%s\n%s\n%s"),*method,*strURIPath,*strQueryString,*CanonicalHeaders,*SignedHeaders,
		*contentSha256) ;
	FString CredentialScope = FString::Printf(TEXT("%s/%s/%s/request"), *xShortData,*strRegion,*strServer);
	FString StringToSign = FString::Printf(TEXT("HMAC-SHA256\n%s\n%s\n%s"),*xdate,*CredentialScope,
		*rtc::tools::Sha256(CanonicalRequest));
	uint8 Digest[32];
	uint8 Digest2[32];
	rtc::tools::HmacSign2(TRANSLATESECRET,xShortData, Digest);
	rtc::tools::HmacSign2(Digest,32,strRegion,Digest2);
	rtc::tools::HmacSign2(Digest2, 32,strServer, Digest);
	rtc::tools::HmacSign2(Digest,32,TEXT("request"), Digest2);
	
	FString Signature =  rtc::tools::HmacSign2(Digest2,32,StringToSign);
	FString Authorization=FString::Printf(TEXT("HMAC-SHA256 Credential=%s/%s, SignedHeaders=%s, Signature=%s"),
		TRANSLATEACCESSKEYID,*CredentialScope,*SignedHeaders, *Signature);
	m_headers.Add(TEXT("Authorization"));
	m_headers.Add(Authorization);
	return true;
}
