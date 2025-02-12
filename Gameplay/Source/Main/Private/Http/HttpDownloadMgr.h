#pragma once

class CHttpDownloadTask;

class FOnHttpDownloadComplete;
class FHttpCoroutineRequest;
class CHttpDownloadMgr
{
private:
	CHttpDownloadMgr();
	~CHttpDownloadMgr();
	TArray<CHttpDownloadTask*>	m_downloads;
	static CHttpDownloadMgr* m_downloadmgrHandle;
	
public:
	
	static CHttpDownloadMgr* Get();
	static void Free();
	bool Download(const FString& strUrl, const FString& savePath, int64 nSize, int32 nThreadCount, int32 nRetryTimes,  FString strMd5, const FOnHttpDownloadComplete& callback);
	bool Download(const FString& strUrl, const FString& savePath, TArray<int64>& nRanges,  int32 nRetryTimes,  FString strMd5, const FOnHttpDownloadComplete& callback);
	bool Download(const FString& strUrl, int32 nRetryTimes,  FString strMd5, const FOnHttpDownloadComplete& callback);
	bool ReDownloadErrorTask();
	bool Stop(const FString& strUrl);
	bool Stop(CHttpDownloadTask* pTask);
	bool Stop();
	void Pause(const FString& strUrl);
	void Continue(const FString& strUrl);
	void Tick(float ticktime);
	void DataWriteToDisk();

	int64 GetHadDownloaded(FString& downloadInfo);

	CHttpDownloadTask* GetTask(const FString& strUrl);

	FString					 m_savePath;

	static bool CheckUrl(const FString& strUrl);
	void AddCoroutineRequest(FHttpCoroutineRequest* pRequest);
	//解析URL，http://www.baidu.com/getpa?aa=bb&c=1; host="www.baidu.com",path="/getpa", queryString="aa=bb&c=1"
	static bool ParseUrl(const FString& strUrl, FString& host, FString& path, FString& queryString);
private:
	TArray<FHttpCoroutineRequest*>	m_coroutineRequestList;
};
