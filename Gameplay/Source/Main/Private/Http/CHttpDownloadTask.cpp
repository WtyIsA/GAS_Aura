#include "CHttpDownloadTask.h"

#include "HttpThreadDownload.h"
#include "UnLuaBase.h"
#include "LuaManger.h"
#include "UtilsProject.h"

CHttpDownloadTask::CHttpDownloadTask()
{
	m_bSavePath = true;
	m_bComplete = false;
	m_bError = false;
	m_bNotify =false;
	m_nRetryTimes = 0;
	m_nErrorCode = 0;
	m_bPause = false;
}

CHttpDownloadTask::~CHttpDownloadTask()
{
	Stop();
	UnLua::DelegateUnBind((FScriptDelegate*) (&m_completeCallback));
}



bool CHttpDownloadTask::Download(const FString& strUrl, const FString& savePath, int64 nSize, int32 nThreadCount, int32 nRetryTimes,  FString strMd5,const FOnHttpDownloadComplete&callback)
{
	m_completeCallback = std::move(callback);
	m_strUrl = strUrl;
	m_bSavePath = true;
	m_strSavePath = savePath;
	m_strMd5 = strMd5;
	m_nRetryTimes = nRetryTimes;
	FString strPath = FPaths::GetPath(m_strSavePath);
	if(!IFileManager::Get().DirectoryExists(*strPath))
	{
		IFileManager::Get().MakeDirectory(*strPath, true);
	}
	if(nThreadCount < 1)
		nThreadCount = 1;
	if(nSize < nThreadCount)
		nThreadCount = 1;
	int nBegin = 0;
	int nPerSize = nSize / nThreadCount;
	
	for(int i = 0; i < nThreadCount - 1; i++)
	{
		CHttpThreadDownload* pDownload = new CHttpThreadDownload(this);
		if(pDownload->Download(strUrl, m_strSavePath,nBegin, nBegin, nBegin + nPerSize - 1, nRetryTimes))
		{
			nBegin += nPerSize;
			m_httpClients.Add(pDownload);
		}else
		{
			Stop();
			return false;
		}
	}
	CHttpThreadDownload* pDownload = new CHttpThreadDownload(this);
	if(pDownload->Download(strUrl, m_strSavePath,nBegin, nBegin, nSize-1, nRetryTimes))
		m_httpClients.Add(pDownload);
	else
		return false;
	return true;
}

bool CHttpDownloadTask::Download(const FString& strUrl, const FString& savePath, TArray<int64>& nRanges,  int32 nRetryTimes,  FString strMd5,const FOnHttpDownloadComplete&callback)
{	
	m_completeCallback = std::move(callback);
	if(nRanges.Num() == 0 || nRanges.Num() % 3 != 0)
		return  false;
	m_strUrl = strUrl;
	m_bSavePath = true;
	m_strSavePath = savePath;
	m_strMd5 = strMd5;
	m_nRetryTimes = nRetryTimes;
	FString strPath = FPaths::GetPath(m_strSavePath);
	if(!IFileManager::Get().DirectoryExists(*strPath))
	{
		IFileManager::Get().MakeDirectory(*strPath, true);
	}
	int nThreadCount = nRanges.Num() / 3;
	int nPos = 0;
	for(int i = 0; i < nThreadCount; i++)
	{
		CHttpThreadDownload* pDownload = new CHttpThreadDownload(this);
		if(pDownload->Download(strUrl, m_strSavePath,nRanges[nPos], nRanges[nPos] + nRanges[nPos+1], nRanges[nPos+2], nRetryTimes))
		{
			m_httpClients.Add(pDownload);
		}
		else
		{
			Stop();
			return  false;
		}
		nPos += 3;
	}
	return  true;
}

bool CHttpDownloadTask::Download(const FString& strUrl, int32 nRetryTimes,  FString strMd5,const FOnHttpDownloadComplete&callback)
{
	CHttpThreadDownload* pDownload = new CHttpThreadDownload(this);
	if(pDownload->Download(strUrl, nRetryTimes))
		m_httpClients.Add(pDownload);
	m_bSavePath = false;
	m_strUrl = strUrl;
	m_strMd5 = strMd5;
	m_nRetryTimes = nRetryTimes;
	m_completeCallback = std::move(callback);
	return true;
}

bool CHttpDownloadTask::Stop()
{
	for (auto httpclient : m_httpClients)
	{
		httpclient->Stop();
		delete httpclient;
	}
	m_httpClients.Empty();

	return true;
}

void CHttpDownloadTask::Pause()
{
	if(m_bPause)
		return;
	for (auto httpclient : m_httpClients)
	{
		httpclient->Pause();
	}
	m_bPause = true;
}

void CHttpDownloadTask::Continue()
{
	if(!m_bPause)
		return;
	for (auto httpclient : m_httpClients)
	{
		httpclient->Continue();
	}
	m_bPause = false;
}

void CHttpDownloadTask::Tick(float ticktime)
{
	bool bError = false;
	bool bComplete = true;
	const TArray<uint8>*	pBinaryContent = nullptr ;
	for(int i = m_httpClients.Num() - 1; i >= 0; i--)
	{
		auto httpclient = m_httpClients[i];
		if(httpclient->_bComplete)
		{
			if(httpclient->_nState != EHttpRequestStatus::Succeeded)
			{				
				if(httpclient->m_nRetryTimes > 0)
				{
					--httpclient->m_nRetryTimes;
					CHttpThreadDownload* pDownload = new CHttpThreadDownload(this);
					if(pDownload->Download(m_strUrl, m_strSavePath,httpclient->_threadAssignPos,  httpclient->_threadAssignPos + httpclient->_hadDownloadLen, httpclient->_fileStopPos, httpclient->m_nRetryTimes))
					{
						bComplete = false;
						m_httpClients[i] = pDownload;
						delete httpclient;
						UE_LOG(GAME, Display, TEXT("[http]CHttpDownloadTask::Tick %s,retrytime:%d, add retry download %p"), *m_strUrl,httpclient->m_nRetryTimes, pDownload);
					}else
					{
						bError = true;
						UE_LOG(GAME, Display, TEXT("[http]CHttpDownloadTask::Tick %s, retry download error"), *m_strUrl);
					}
					
				}
				else
				{
					bError = true;
					m_nErrorCode = EHTTP_ERROR_DOWNLOAD;
				}
			}
			else if(!m_bSavePath)
			{
				m_strContent = httpclient->m_strContent;
				pBinaryContent = &httpclient->m_pBinaryContent;
			}
		}else
		{
			bComplete = false;
		}
	}
	if(bComplete && !m_bError && !bError && m_strMd5.Len() == 32)
	{
		if(m_bSavePath)
		{
			FString strmd5 = FUtilsProject::GetFileMd5(m_strSavePath);
			if(!m_strMd5.Equals(strmd5,ESearchCase::IgnoreCase ))
			{
				UE_LOG(GAME, Error, TEXT("[http]%s download md5 error %s != %s, url=%s"), *m_strSavePath, *m_strMd5, *strmd5, *m_strUrl);
				bError = true;
				m_nErrorCode = EHTTP_ERROR_MD5;
			}
		}
		else
		{
			FString strmd5 = FUtilsProject::GetContentMd5(pBinaryContent);
			if(!m_strMd5.Equals(strmd5,ESearchCase::IgnoreCase ))
			{
				UE_LOG(GAME, Error, TEXT("[http]%s download md5 error %s != %s, url=%s"), *m_strSavePath, *m_strMd5, *strmd5, *m_strUrl);
				bError = true;
				m_nErrorCode = EHTTP_ERROR_MD5;
			}
		}
		if(bError)
		{
			for(auto item: m_httpClients)
			{
				item->_hadDownloadLen = 0;
				item->m_nRetryTimes = 0;
				item->_nState = EHttpRequestStatus::NotStarted;
			}
		}
	}
	//if(bError)
	//	Stop();
	m_bError = m_bError | bError;
	m_bComplete = bComplete;
}

void CHttpDownloadTask::WriteDataToDisk()
{
	for(auto item: m_httpClients)
	{
		item->WriteData();
	}
}

FString& CHttpDownloadTask::GetMd5()
{
	return  m_strMd5;
}

int64 CHttpDownloadTask::GetHadDownloaded(FString& downloadInfo)
{
	if(m_httpClients.Num() == 0)
	{
		downloadInfo = TEXT("{}");
		return  0;
	}
	int64 nLen = 0;
	downloadInfo = TEXT("[");
	//for(auto item: m_httpClients)
	for(int i = 0; i < m_httpClients.Num() - 1; i++)
	{
		CHttpThreadDownload* item = m_httpClients[i];
		nLen += item->_hadDownloadLen;
		downloadInfo += FString::Printf(TEXT("[%lld,%lld,%lld],"), item->_threadAssignPos, item->_hadDownloadLen, item->_fileStopPos);
	}
	{
		CHttpThreadDownload* item = m_httpClients.Last();
		nLen += item->_hadDownloadLen;
		downloadInfo += FString::Printf(TEXT("[%lld,%lld,%lld]]"), item->_threadAssignPos, item->_hadDownloadLen, item->_fileStopPos);
	}
	return  nLen;
}

bool CHttpDownloadTask::ReDownload()
{
	for(auto item: m_httpClients)
	{
		item->m_nRetryTimes = m_nRetryTimes + 1;
	}
	m_bError = false;
	m_bNotify = false;
	return  false;
}
