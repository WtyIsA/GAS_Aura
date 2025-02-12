#pragma once
#include "LuaManger.h"
enum EHTTP_ERROR_TYPE
{
	EHTTP_ERROR_DOWNLOAD,  //下载失败
	EHTTP_ERROR_MD5 = 5,  //md5 不对
};

class CHttpThreadDownload;

class CHttpDownloadTask
{
public:
	CHttpDownloadTask();
	~CHttpDownloadTask();

	bool Download(const FString& strUrl, const FString& savePath, int64 nSize, int32 nThreadCount, int32 nRetryTimes,  FString strMd5,const FOnHttpDownloadComplete&callback);
	bool Download(const FString& strUrl, const FString& savePath, TArray<int64>& nRanges,  int32 nRetryTimes,  FString strMd5,const FOnHttpDownloadComplete&callback);
	bool Download(const FString& strUrl, int32 nRetryTimes,  FString strMd5,const FOnHttpDownloadComplete&callback);
	bool ReDownload();
	bool Stop();
	void Pause();
	void Continue();
	void Tick(float ticktime);
	void WriteDataToDisk();
	int64 GetHadDownloaded(FString& downloadInfo);
	FString& GetMd5();
private:
	FString m_strSavePath;
	FString m_strContent;
	FString m_strUrl;
	FString m_strMd5;
	bool		m_bComplete;
	bool		m_bError;
	bool		m_bSavePath;
	bool		m_bNotify; //失败是否已通知，只通知一遍
	bool		m_bPause;
	int			m_nRetryTimes;
	int			m_nErrorCode;
	FOnHttpDownloadComplete		m_completeCallback;
	TArray<CHttpThreadDownload*> m_httpClients;

	friend class  CHttpDownloadMgr;
	
};
