#pragma once
#include "Interfaces/IHttpRequest.h"
class IFileHandle;

class CHttpDownloadTask;
class CHttpThreadDownload
{
public:
	CHttpThreadDownload(CHttpDownloadTask*);
	~CHttpThreadDownload();

	friend class CHttpDownloadTask;
public:
	/*
	 * assignPos 分配此线程下载的开始位置
	 * nDownloadPos 此次下载的开始位置，nDownloadPos-assignPos 为此线程已经下载完成大小
	 * nStopPos  分配此线程下载的结束大小，为 “0” 表示到文件结尾  bytes=[nDownloadPos,nStopPos]
	 */
	bool Download(const FString& strUrl,  FString savePath, int64 assignPos, int64 nDownloadPos, int64 nStopPos, int32 nRetryTimes = 5);
	bool Download(const FString& strUrl, int32 nRetryTimes = 5);
	bool Stop();
	void WriteData();
	void Pause();
	void Continue();
private:
	
	void OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void OnRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived);

	void OnWriteToDisk(FHttpRequestPtr Request, int8* write_data, int32 datalen);
private:
	TArray<uint8>	m_pBinaryContent;
	FString m_strSavePath;
	IFileHandle* _fileHandle;
	int64		 _threadAssignPos; //线程分配的开始位置
	int64		 _hadDownloadLen;
	int64		 _fileStopPos;
	bool		_bComplete;
	EHttpRequestStatus::Type  _nState;
	bool		_bSavePath;
	int			m_nRetryTimes;
	FString		m_strContent;
	CHttpDownloadTask*	m_task;
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> _requestHandle;
	FString		m_strUrl;
};
