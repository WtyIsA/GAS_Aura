#include "HttpThreadDownload.h"
#include "CHttpDownloadTask.h"
#include "HttpManager.h"
#include "HttpModule.h"
#include "LuaManger.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"

CHttpThreadDownload::CHttpThreadDownload(CHttpDownloadTask* pTask):
	_fileHandle(nullptr)
	,_bComplete(false)
{
	_nState = EHttpRequestStatus::NotStarted;
	m_nRetryTimes = 5;
	m_task = pTask;
	_bSavePath = false;
	_hadDownloadLen = 0;
	_threadAssignPos = 0;
	_requestHandle = nullptr;
}

CHttpThreadDownload::~CHttpThreadDownload()
{
	Stop();
}


void CHttpThreadDownload::OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	_bComplete = true;
	_nState = HttpRequest.Get()->GetStatus();
	if (!bSucceeded || !HttpResponse || !EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		UE_LOG(GAME, Error, TEXT("[http] CHttpThreadDownload download error, %s, code=%d"), HttpRequest?*HttpRequest->GetURL():TEXT("HttpRequest is null"), HttpResponse?HttpResponse->GetResponseCode():-1111);
		bSucceeded = false;
		_nState = EHttpRequestStatus::Failed;
	}
	if(bSucceeded)
	{
		if(_bSavePath)
		{
			HttpRequest.Get()->DownloadedDataWriteToDisk();
			if (_fileHandle != nullptr)
			{
				_fileHandle->Flush();
				delete _fileHandle;
				_fileHandle = nullptr;
			}
		}
		else
		{
			m_strContent = HttpResponse.Get()->GetContentAsString();
			if(m_task && m_task->GetMd5().Len() == 32)
			{
				m_pBinaryContent.Empty();
				m_pBinaryContent.Append(HttpResponse.Get()->GetContent());
			}
		}
	}
	else
	{
		Stop();
	}
	_requestHandle.Reset();
}

void CHttpThreadDownload::OnRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived)
{
	
}

void CHttpThreadDownload::OnWriteToDisk(FHttpRequestPtr Request, int8* write_data, int32 datalen)
{
	if (_fileHandle == NULL)
		return;
	bool bWrite = _fileHandle->Write((const uint8*)write_data, datalen);
	_hadDownloadLen += datalen;
	
}

bool CHttpThreadDownload::Download(const FString& strUrl, FString savePath,int64 assignPos, int64 nDownloadPos, int64 nStopPos, int32 nRetryTimes)
{
	if(nRetryTimes < 0)
		nRetryTimes = 5;
	_threadAssignPos = assignPos;
	_hadDownloadLen = nDownloadPos - assignPos;
	_fileStopPos = nStopPos;
	if(_fileHandle != nullptr)
	{
		_fileHandle->Flush();
		delete _fileHandle;
	}
	m_strSavePath = savePath;
	if(nStopPos >0 && nDownloadPos > nStopPos)
	{
		_bComplete = true;
		_nState = EHttpRequestStatus::Succeeded;
		return true;
	}
	IPlatformFile& platformFile = IPlatformFile::GetPlatformPhysical();
	_fileHandle = platformFile.OpenWrite(*m_strSavePath, true, true);
	if(_fileHandle == nullptr)
	{
		UE_LOG(GAME, Error, TEXT("[http]platformFile.OpenWrite %s error, save path %s"), *strUrl, *m_strSavePath);
		return false;
	}
	m_strUrl = strUrl;
	_fileHandle->Seek(nDownloadPos);
	_bSavePath = true;
	m_nRetryTimes = nRetryTimes;
	
	_requestHandle = FHttpModule::Get().CreateRequest();	
	_requestHandle->SetURL(strUrl);
	_requestHandle->SetVerb(TEXT("GET"));
	FString strRange ;
	if(nStopPos > 0)
		strRange = FString::Printf(TEXT("bytes=%lld-%lld"), nDownloadPos, nStopPos);
	else
		strRange = FString::Printf(TEXT("bytes=%lld-"), nDownloadPos);
	_requestHandle->SetHeader(TEXT("Range"),*strRange);
	_requestHandle->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
	_requestHandle->OnProcessRequestComplete().BindRaw(this, &CHttpThreadDownload::OnRequestComplete);
	//requestHandle->OnRequestProgress().BindRaw(this, &CHttpThreadDownload::OnRequestProgress);
	_requestHandle->OnWriteDataToDisk().BindRaw(this, &CHttpThreadDownload::OnWriteToDisk);
	_requestHandle->SetTickWriteData(false);
	_requestHandle->ProcessRequest();

	return true;
}

bool CHttpThreadDownload::Download(const FString& strUrl, int32 nRetryTimes)
{
	if(nRetryTimes < 0)
		nRetryTimes = 5;
	_bSavePath = false;
	m_nRetryTimes = nRetryTimes;
	_requestHandle = FHttpModule::Get().CreateRequest();	
	_requestHandle->SetURL(strUrl);
	_requestHandle->SetVerb(TEXT("GET"));
	_requestHandle->OnProcessRequestComplete().BindRaw(this, &CHttpThreadDownload::OnRequestComplete);
	_requestHandle->OnRequestProgress().BindRaw(this, &CHttpThreadDownload::OnRequestProgress);	
	_requestHandle->ProcessRequest();
	m_strUrl = strUrl;
	return true;
}

bool CHttpThreadDownload::Stop()
{
	if(_fileHandle != nullptr)
	{
		_fileHandle->Flush();
		delete _fileHandle;
		_fileHandle = nullptr;	
		
	}
	if(_requestHandle.IsValid())
	{
		_requestHandle->OnWriteDataToDisk().Unbind();
		_requestHandle->OnProcessRequestComplete().Unbind();
		_requestHandle->OnRequestProgress().Unbind();
		_requestHandle->CancelRequest();
	}
	_requestHandle.Reset();
	
	return true;
}

void CHttpThreadDownload::Pause()
{
	Stop();
}

void CHttpThreadDownload::Continue()
{
	if(_bComplete)
		return;
	if(!_bSavePath)
	{
		Download(m_strUrl, m_nRetryTimes);
	}
	else
	{
		Download(m_strUrl, m_strSavePath, _threadAssignPos, _hadDownloadLen + _threadAssignPos, _fileStopPos,m_nRetryTimes);
	}
}



void CHttpThreadDownload::WriteData()
{
	if(_bComplete || _requestHandle == nullptr)
		return;
	if(_requestHandle->IsUserEngineTickWriteData())
		return;
	_requestHandle->DownloadedDataWriteToDisk();
}

