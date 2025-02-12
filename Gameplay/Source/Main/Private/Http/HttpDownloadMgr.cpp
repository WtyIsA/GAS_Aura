#include "HttpDownloadMgr.h"


#include "CHttpDownloadTask.h"
#include "HttpCoroutineRequest.h"
#include "HttpTranslateTextRequest.h"
#include "LuaManger.h"
#include "LuaMessageTableOptInC.h"
#include "UnLuaBase.h"
#include "UnLuaEx.h"

CHttpDownloadMgr* CHttpDownloadMgr::m_downloadmgrHandle = nullptr;

CHttpDownloadMgr::CHttpDownloadMgr()
{
	m_savePath = UnLua::GetDonwloadResPath();
}

CHttpDownloadMgr::~CHttpDownloadMgr()
{
	for(auto item : m_downloads)
	{
		item->Stop();
		delete item;
	}
	m_downloads.Empty();
}


CHttpDownloadMgr* CHttpDownloadMgr::Get()
{
	if(m_downloadmgrHandle == nullptr)
	{
		m_downloadmgrHandle = new CHttpDownloadMgr();		
	}
	return  m_downloadmgrHandle;
}

void CHttpDownloadMgr::Free()
{
	if(m_downloadmgrHandle)
	{
		delete m_downloadmgrHandle;
		m_downloadmgrHandle = nullptr;
	}
}

CHttpDownloadTask* CHttpDownloadMgr::GetTask(const FString& strUrl)
{
	for(auto item : m_downloads)
	{
		if(item->m_strUrl == strUrl)
			return item;
	}
	return  nullptr;
}

bool CHttpDownloadMgr::Download(const FString& strUrl, const FString& savePath, int64 nSize, int32 nThreadCount, int32 nRetryTimes,  FString strMd5, const FOnHttpDownloadComplete& callback)
{
	if(!CheckUrl(strUrl))
		return false;
	CHttpDownloadTask * pTask = GetTask(strUrl);
	if(pTask)
	{
		if(pTask->m_bError)
			Stop(pTask);
		else if(pTask->m_bPause)
		{
			pTask->Continue();
			return true;
		}
		else
			return false;
	}
	
	FString strSavePath = "";
	if(savePath.StartsWith(FPaths::ProjectPersistentDownloadDir()))
		strSavePath = savePath + TEXT(".tmp");
	else
		strSavePath = FString::Printf(TEXT("%s/%s.tmp"),*m_savePath , *savePath );
	pTask = new CHttpDownloadTask();
	if(pTask->Download(strUrl, strSavePath, nSize, nThreadCount, nRetryTimes, strMd5,callback))
		m_downloads.Add(pTask);
	else
	{
		if(pTask->m_completeCallback.IsBound())
			pTask->m_completeCallback.Execute(strUrl, false, TEXT(""), true,0);
		else
		{
			ULuaManger::Get()->OnDownloadComplete(strUrl, false, TEXT(""), true);
		}
		delete pTask;
		return  false;
	}
	
	return true;
}

bool CHttpDownloadMgr::Download(const FString& strUrl, const FString& savePath, TArray<int64>& nRanges,  int32 nRetryTimes,  FString strMd5, const FOnHttpDownloadComplete& callback)
{
	if(!CheckUrl(strUrl))
		return false;
	CHttpDownloadTask * pTask = GetTask(strUrl);
	if(pTask)
	{
		if(pTask->m_bError)
			Stop(pTask);
		else if(pTask->m_bPause)
		{
			pTask->Continue();
			return true;
		}
		else
			return false;
	}
	FString strSavePath = "";
	if(savePath.StartsWith(FPaths::ProjectPersistentDownloadDir()))
		strSavePath = savePath + TEXT(".tmp");
	else
		strSavePath = FString::Printf(TEXT("%s/%s.tmp"),*m_savePath , *savePath );
	pTask = new CHttpDownloadTask();
	if(pTask->Download(strUrl, strSavePath, nRanges,  nRetryTimes, strMd5,callback))
		m_downloads.Add(pTask);
	else
	{
		if(pTask->m_completeCallback.IsBound())
			pTask->m_completeCallback.Execute(strUrl, false, TEXT(""), true,0);
		else
		{
			ULuaManger::Get()->OnDownloadComplete(strUrl, false, TEXT(""), true);
		}
		delete pTask;
		return  false;
	}
	
	return true;
}

bool CHttpDownloadMgr::Download(const FString& strUrl, int32 nRetryTimes,  FString strMd5, const FOnHttpDownloadComplete& callback)
{
	if(!CheckUrl(strUrl))
		return false;
	CHttpDownloadTask * pTask = GetTask(strUrl);
	if(pTask)
	{
		if(pTask->m_bPause)
		{
			pTask->Continue();
			return true;
		}
		return false;
	}
	pTask = new CHttpDownloadTask();
	if(pTask->Download(strUrl, nRetryTimes, strMd5,callback))
		m_downloads.Add(pTask);
	else
	{
		if(pTask->m_completeCallback.IsBound())
			pTask->m_completeCallback.Execute(strUrl, false, TEXT(""), false, 0);
		else
		{
			ULuaManger::Get()->OnDownloadComplete(strUrl, false, TEXT(""), false);
		}
		delete pTask;
		return  false;
	}
	return true;
}

bool CHttpDownloadMgr::ReDownloadErrorTask()
{
	for(auto pTask:m_downloads)
	{
		if(pTask->m_bError && pTask->m_bSavePath)
		{
			pTask->ReDownload();
		}
	}
	return  true;
}


bool CHttpDownloadMgr::Stop(CHttpDownloadTask* pTask)
{
	if(pTask)
	{
		pTask->Stop();
		m_downloads.Remove(pTask);
		delete pTask;
		return true;
	}
	return  false;
}

bool CHttpDownloadMgr::Stop(const FString& strUrl)
{
	CHttpDownloadTask * pTask = GetTask(strUrl);
	return  Stop(pTask);
}

bool CHttpDownloadMgr::Stop()
{
	for(int i = m_downloads.Num() - 1; i >= 0; i--)
	{
		CHttpDownloadTask* pTask = m_downloads[i];
		pTask->Stop();
		m_downloads.RemoveAt(i);
		delete pTask;
	}
	return  true;
}

void CHttpDownloadMgr::Pause(const FString& strUrl)
{
	CHttpDownloadTask * pTask = GetTask(strUrl);
	if(pTask)
		pTask->Pause();
}

void CHttpDownloadMgr::Continue(const FString& strUrl)
{
	CHttpDownloadTask * pTask = GetTask(strUrl);
	if(pTask)
		pTask->Continue();
}


void CHttpDownloadMgr::Tick(float ticktime)
{
	for(int i = m_downloads.Num() - 1; i >= 0; i--)
	{
		CHttpDownloadTask* pTask = m_downloads[i];
		pTask->Tick(ticktime);
		if(pTask->m_bError)
		{
			if(!pTask->m_bNotify)
			{
				pTask->m_bNotify = true;
				if(pTask->m_completeCallback.IsBound())
					pTask->m_completeCallback.Execute(pTask->m_strUrl, false, TEXT(""), pTask->m_bSavePath, pTask->m_nErrorCode);
				else
					ULuaManger::Get()->OnDownloadComplete(pTask->m_strUrl, false, TEXT(""), pTask->m_bSavePath, pTask->m_nErrorCode);
			}
			if(!pTask->m_bSavePath)
			{
				delete pTask;
				m_downloads.RemoveAt(i);
			}
		}
		else if(pTask->m_bComplete)
		{
			if(pTask->m_bSavePath)
			{
				FString savename = pTask->m_strSavePath;
				FString correntName = savename.Left(savename.Len() - 4);
				IFileManager::Get().Move(*correntName, *savename);
				if(pTask->m_completeCallback.IsBound())
					pTask->m_completeCallback.Execute(pTask->m_strUrl,true, correntName, pTask->m_bSavePath, 0);
				else
					ULuaManger::Get()->OnDownloadComplete(pTask->m_strUrl,true, correntName, pTask->m_bSavePath);
			}
			else
			{
				if(pTask->m_completeCallback.IsBound())
					pTask->m_completeCallback.Execute(pTask->m_strUrl,true, pTask->m_strContent, pTask->m_bSavePath, 0);
				else
					ULuaManger::Get()->OnDownloadComplete(pTask->m_strUrl,true, pTask->m_strContent, pTask->m_bSavePath);
			}
			delete pTask;
			m_downloads.RemoveAt(i);
		}
	}

	for(int i = 0; i < m_coroutineRequestList.Num(); i++)
	{
		m_coroutineRequestList[i]->Process();
	}
	m_coroutineRequestList.Empty();
	//DataWriteToDisk();
}

void CHttpDownloadMgr::DataWriteToDisk()
{
	for(auto item : m_downloads)
	{
		if(item->m_bSavePath)
			item->WriteDataToDisk();
	}
}

int64 CHttpDownloadMgr::GetHadDownloaded(FString& downloadInfo)
{
	if(m_downloads.Num() == 0)
	{
		downloadInfo = TEXT("{}");
		return  0;
	}
	int64 nDownloadLen = 0;
	downloadInfo = TEXT("{");
	FString itemInfo;
	for(int i = 0; i < m_downloads.Num() - 1; i++)
	{
		auto item = m_downloads[i];
		if(item->m_httpClients.Num() == 0 || !item->m_bSavePath)
			continue;
		nDownloadLen += item->GetHadDownloaded(itemInfo);
		downloadInfo += FString::Printf(TEXT("\"%s\":%s,"), *item->GetMd5(), *itemInfo);
	}
	{
		auto item = m_downloads.Last();
		if(item->m_httpClients.Num() > 0 && item->m_bSavePath)
		{
			nDownloadLen += item->GetHadDownloaded(itemInfo);
			downloadInfo += FString::Printf(TEXT("\"%s\":%s}"), *item->GetMd5(), *itemInfo);
		}
	}
	return  nDownloadLen;
}

bool CHttpDownloadMgr::CheckUrl(const FString& strUrl)
{
	if(strUrl.Len() < 5)
	{
		UE_LOG(GAME,Display,TEXT("[http]url error %s"), *strUrl);
		return false;
	}
	if(strUrl.StartsWith(TEXT("http")) || strUrl.StartsWith(TEXT("ftp")) )
		return true;
	UE_LOG(GAME,Error,TEXT("[http]url error %s"), *strUrl);
	return false;
}

void CHttpDownloadMgr::AddCoroutineRequest(FHttpCoroutineRequest* pRequest)
{
	if(pRequest)
		m_coroutineRequestList.Add(pRequest);
}

bool CHttpDownloadMgr::ParseUrl(const FString& strUrl, FString& host, FString& path, FString& queryString)
{
	if(!strUrl.StartsWith(TEXT("http")) && !strUrl.StartsWith(TEXT("ftp")) )
		return false;
	int nPos = strUrl.Find(TEXT("https://"));
	int nHostStart = 8;
	if(nPos < 0)
	{
		nPos = strUrl.Find(TEXT("http://"));
		nHostStart = 7;
	}
	if(nPos < 0)
	{
		nPos = strUrl.Find(TEXT("ftp://"));
		nHostStart = 6;
	}
	if(nPos < 0)
		return false;
	path.Empty();
    queryString.Empty();
	int nPos2 = strUrl.Find(TEXT("/"), ESearchCase::CaseSensitive, ESearchDir::FromStart, nPos + 8);
	if(nPos2 < 0)
	{
		host = strUrl.Mid(nHostStart);		
		return true;
	}
	host = strUrl.Mid(nHostStart, nPos2 - nHostStart);		
	int nPos3 = 0;
	if(!strUrl.FindChar(TEXT('?'), nPos3))
	{
		path = strUrl.Mid(nPos2);
	}
	else
	{
		path = strUrl.Mid(nPos2, nPos3-nPos2);
		queryString = strUrl.Mid(nPos3+1);
	}
	return true;
}



int LuaHttpRequest(lua_State* L)
{	
	int nParamNum = lua_gettop(L);
	if(nParamNum < 3)
	{
		UE_LOG(GAME,Error,TEXT("[http]HttpRequest error param less 3  num=%d"),nParamNum);
		return CLuaMessageTableOptInC::PrintLuaTrackbackAndPushNil(nullptr, L);
	}
	for(int i = 1; i <= nParamNum; i++)
	{
		if(lua_type(L,i) != LUA_TSTRING)
		{
			UE_LOG(GAME,Error,TEXT("[http]HttpRequest error param%d is not string"),i);
			return CLuaMessageTableOptInC::PrintLuaTrackbackAndPushNil(nullptr, L);
		}  
	}
	FString strUrl(UTF8_TO_TCHAR(lua_tostring(L,1)));
	FString strMethod(UTF8_TO_TCHAR(lua_tostring(L,2)));
	FString strContent(UTF8_TO_TCHAR(lua_tostring(L,3)));
	TArray<FString> header;
	for(int i = 4; i <= nParamNum; i++)
	{
		FString strHead(UTF8_TO_TCHAR(lua_tostring(L, i)));
		header.Add(strHead);
	}
	FHttpCoroutineRequest* pHttpRequest = new FHttpCoroutineRequest(true);
	if(!pHttpRequest->Init(L, strUrl, strMethod, strContent, header))
	{
		delete pHttpRequest;
		return 0;
	}
	CHttpDownloadMgr::Get()->AddCoroutineRequest(pHttpRequest);
	int nToRet =  lua_yield(L,0);
	return 0;
}

int LuaHttpTranslateTextRequest(lua_State* L)
{	
	int nParamNum = lua_gettop(L);
	if(nParamNum < 3)
	{
		UE_LOG(GAME,Error,TEXT("[http]LuaHttpTranslateTextRequest error param less 3  num=%d"),nParamNum);
		return CLuaMessageTableOptInC::PrintLuaTrackbackAndPushNil(nullptr, L);
	}
	for(int i = 1; i <= nParamNum; i++)
	{
		if(lua_type(L,i) != LUA_TSTRING)
		{
			UE_LOG(GAME,Error,TEXT("[http]LuaHttpTranslateTextRequest error param%d is not string"),i);
			return CLuaMessageTableOptInC::PrintLuaTrackbackAndPushNil(nullptr, L);
		}  
	}
	FString strUrl(UTF8_TO_TCHAR(lua_tostring(L,1)));
	FString strMethod(UTF8_TO_TCHAR(lua_tostring(L,2)));
	FString strContent(UTF8_TO_TCHAR(lua_tostring(L,3)));
	TArray<FString> header;
	for(int i = 4; i <= nParamNum; i++)
	{
		FString strHead(UTF8_TO_TCHAR(lua_tostring(L, i)));
		header.Add(strHead);
	}
	FHttpTranslateTextRequest* pHttpRequest = new FHttpTranslateTextRequest(true);
	if(!pHttpRequest->Init(L, strUrl, strMethod, strContent, header))
	{
		delete pHttpRequest;
		return 0;
	}
	CHttpDownloadMgr::Get()->AddCoroutineRequest(pHttpRequest);
	int nToRet =  lua_yield(L,0);
	return 0;
}


static const luaL_Reg FLuaHttpRequestLib[] =
{
	{"Request", LuaHttpRequest},
	{"TranslateText",LuaHttpTranslateTextRequest},
	{ nullptr, nullptr }
};

EXPORT_UNTYPED_CLASS(FLuaHttpRequest, false, FLuaHttpRequestLib)
IMPLEMENT_EXPORTED_CLASS(FLuaHttpRequest)