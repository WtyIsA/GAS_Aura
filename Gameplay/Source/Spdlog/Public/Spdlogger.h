#pragma once
#include "CoreMinimal.h"


#include <memory>
#include <string>


#include "Spdlogger.generated.h"
struct Table;

struct USpdlogInitParams
{
	USpdlogInitParams()
	{
		m_fileSize = 104857600;//100mb
		m_maxFileCount = 100;
		m_serverId = 0;
		m_flushLevel = 4;//err
		m_logLevel = 2; //info
		m_pattens = "%Y%m%d-%T:%e %l %v";//20211115-10:32:08 INFO [module]log
	}
	std::string		m_logName;  //必填
	std::string		m_filePath; //必填
	uint32			m_fileSize;
	uint32			m_maxFileCount;
	int				m_serverId;
	uint8			m_flushLevel;
	uint8			m_logLevel;
	std::string		m_pattens;
};

/** lua调用 local a=USpdlogger(t).
表t的字段说明：
name string 日志名，必填，区分不同的USpdlogger
path string 日志全路径，必填。
filesize number @int32 每个文件大小，默认 100M
maxfiles number @int32 文件最大数量，默认 100
flushlevel  number @uint8 自动刷新级别，默认 4：err
loglevel  number @uint8 日志记录级别，大于等于该值记录，默认 2：info
pattern string 日志格式，默认 "%Y%m%d-%T:%e %l %v";//20211115-10:32:08:654 info [module]log
*/
			  
UCLASS()
class SPDLOG_API USpdlogger: public UObject
{
	GENERATED_BODY()
public:
	bool Init(const Table* pTable);

	bool Init(const USpdlogInitParams& params);

	/*日志写盘，主动调用，只有日志级别大于flushlevel才会自动写盘*/
	UFUNCTION()
	void Flush()const;
	/*
	 nLevel : 0 :trace; 1:debug; 2:info; 3 warn; 4:err; 5:critical
	 */
	UFUNCTION()
	void Log(int nLevel,const FString& module,const FString& logContent)const;

	void LogJson(int nLevel,const FString& module,const FString& strJson);
	/*关闭该日志, 成员函数，用":"*/
	UFUNCTION()
    void Close();
	/*关闭所有日志， 静态函数，用"."*/
	UFUNCTION()
    static  void CloseAll();
private:
	std::shared_ptr<void>   m_handle;
	int32					m_serverId;
	int64					m_logId;
};