#include "Spdlogger.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "LuaMessageTableOptInC.h"
#include "UnLuaEx.h"
#include "LuaTableToJson.h"

#define  SPDLOG_PATH				"path"				//日志路径，全目录路径
#define  SPDLOG_FILESIZE			"filesize"			//每个日志文件大小
#define  SPDLOG_PROJECT				"name"				//日志里的项目名，对应 EVO。
#define  SPDLOG_MAXFILE_COUNT		"maxfiles"			//文件夹下日志文件最大数量，超过时删除最老的
#define  SPDLOG_FLUSHLEVEL			"flushlevel"		//大于等于该日志level，会自动刷新
#define  SPDLOG_LOGLEVEL			"loglevel"			//大于等于该level才会打日志
#define  SPDLOG_PATTERN				"pattern"			//设置日志格式
#define  SPDLOG_SERVERID			"serverid"			//设置日志格式

bool USpdlogger::Init(const USpdlogInitParams& params)
{
	if(params.m_filePath.length() == 0 || params.m_fileSize == 0 || params.m_maxFileCount == 0)
		return false;
	std::shared_ptr<spdlog::logger> handle = spdlog::details::registry::instance().get(params.m_logName);
	if(handle == nullptr)
		handle = spdlog::create_async<spdlog::sinks::rotating_file_sink_mt>(params.m_logName,params.m_filePath,params.m_fileSize, params.m_maxFileCount);
	else
	{
		UE_LOG(LogCore,Error,TEXT("[spdlog]init error,logname repeat,%s"), UTF8_TO_TCHAR(params.m_logName.c_str()));
		return false;
	}
	m_handle = handle;
	handle->flush_on((spdlog::level::level_enum)params.m_flushLevel);
	handle->set_level((spdlog::level::level_enum)params.m_logLevel);
	handle->set_formatter(std::unique_ptr<spdlog::formatter>(new spdlog::pattern_formatter(params.m_pattens)));
	m_logId = 0;
	m_serverId = params.m_serverId;
	return true;
}

bool USpdlogger::Init(const Table* luaTable)
{
	if(luaTable == nullptr)
	{
		UE_LOG(LogCore, Fatal, TEXT("[spdlog]Init param luaTable is null"));
		return false;
	}
	USpdlogInitParams logParams;
	const char* log_name=CLuaMessageTableOptInC::GetTValueStr(luaTable, SPDLOG_PROJECT);
	if(log_name != nullptr)
		logParams.m_logName = log_name;
	else
	{
		UE_LOG(LogCore, Display, TEXT("[spdlog]create sqplogger,project name is empty"));
		return false;
	}
	const char* pFilePath = CLuaMessageTableOptInC::GetTValueStr(luaTable, SPDLOG_PATH);
	if(pFilePath == nullptr)
	{
		UE_LOG(LogCore, Fatal, TEXT("[spdlog]Init pFilePath not set"));
		return false;
	}
	logParams.m_filePath = pFilePath;
	int32 filesize = CLuaMessageTableOptInC::GetNumber(luaTable,SPDLOG_FILESIZE);
	if(filesize > 0)
		logParams.m_fileSize = filesize;
		
	int max_files = CLuaMessageTableOptInC::GetNumber(luaTable,SPDLOG_MAXFILE_COUNT);
	if(max_files > 0)
		logParams.m_maxFileCount = max_files;
	
	CLuaMessageTableOptInC::GetNumber(luaTable,SPDLOG_FLUSHLEVEL, logParams.m_flushLevel);
	if(logParams.m_flushLevel > spdlog::level::level_enum::critical)
		logParams.m_flushLevel = spdlog::level::level_enum::err;
	
	CLuaMessageTableOptInC::GetNumber(luaTable,SPDLOG_LOGLEVEL, logParams.m_logLevel);
	if(logParams.m_logLevel > spdlog::level::level_enum::critical)
		logParams.m_logLevel = spdlog::level::level_enum::info;

	const char* pPattens = CLuaMessageTableOptInC::GetTValueStr(luaTable, SPDLOG_PATTERN);
	if(pPattens)
	{
		logParams.m_pattens = pPattens;
	}
	CLuaMessageTableOptInC::GetNumber(luaTable,SPDLOG_SERVERID, logParams.m_serverId);
	return Init(logParams);	
}

void USpdlogger::Log(int nLevel,const FString& module,const FString& logContent) const
{
	try
	{
		if (m_handle)
		{
			if (nLevel < spdlog::level::trace || nLevel > spdlog::level::critical)
			{
				UE_LOG(LogCore, Error, TEXT("[spdlog]nlevel %d error.[0,5]"), nLevel);
				return;
			}
			FString strLog = FString::Printf(TEXT("[%s]%s"), *module, *logContent);
			((spdlog::logger*)m_handle.get())->log((spdlog::level::level_enum)nLevel, TCHAR_TO_UTF8(*strLog));
		}
	}
	catch (spdlog::spdlog_ex e)
	{
		UE_LOG(LogCore,Error,TEXT("[spdlog]log error,%s"), UTF8_TO_TCHAR(e.what()))
	}
	catch (...)
	{
		UE_LOG(LogCore,Error,TEXT("[spdlog]log error"));
	}
	
}

void USpdlogger::LogJson(int nLevel,const FString& module,const FString& strJson)
{
	try
	{
		if (m_handle)
		{
			if (nLevel < spdlog::level::trace || nLevel > spdlog::level::critical)
			{
				UE_LOG(LogCore, Error, TEXT("[spdlog]nlevel %d error.[0,5]"), nLevel);
				return;
			}
			m_logId++;
			//FString strLog = FString::Printf(TEXT("[%s] %d:%lld | %s"), *module,m_serverId, m_logId,  *strJson);
			((spdlog::logger*)m_handle.get())->log((spdlog::level::level_enum)nLevel, TCHAR_TO_UTF8(*strJson));
		}
	}
	catch (spdlog::spdlog_ex e)
	{
		UE_LOG(LogCore,Error,TEXT("[spdlog]log error,%s"), UTF8_TO_TCHAR(e.what()))
	}
	catch (...)
	{
		UE_LOG(LogCore,Error,TEXT("[spdlog]log error"));
	}
	
}

void USpdlogger::Flush() const
{
	try
	{
		if(m_handle)
        	((spdlog::logger*)m_handle.get())->flush();
	}
	catch (spdlog::spdlog_ex e)
	{
		UE_LOG(LogCore,Error,TEXT("[spdlog]Flush error,%s"), UTF8_TO_TCHAR(e.what()))
	}
	catch (...)
	{
		UE_LOG(LogCore,Error,TEXT("[spdlog]Flush error"));
	}
}

void USpdlogger::Close()
{
	if(m_handle)
	{
		Flush();
		UE_LOG(LogCore,Display,TEXT("[spdlog]Close(%s)"), UTF8_TO_TCHAR(((spdlog::logger*)m_handle.get())->name().c_str()));
		spdlog::drop(((spdlog::logger*)m_handle.get())->name());
		m_handle.reset();
		m_handle = nullptr;
	}
}

void USpdlogger::CloseAll() 
{
	spdlog::details::registry::instance().flush_all();
	spdlog::details::registry::instance().shutdown();
	UE_LOG(LogCore,Display,TEXT("[spdlog]CloseAll"));
}



static  int32 Spdlogger_New(lua_State *L)
{
	const int32 nParams = lua_gettop(L);
	if (nParams < 2 || lua_type(L, 2) != LUA_TTABLE)
	{
		UE_LOG(LogCore, Error, TEXT("[spdlog]Spdlogger_New: params num error"));
		return 0;
	}
	Table* luaTable = (Table*)lua_topointer(L,2);	
	USpdlogger* pLogObj = NewObject<USpdlogger>();
	if(pLogObj->Init(luaTable))
	{
		UnLua::PushUObject(L, pLogObj);
		return  1;
	}
	return 0;
}

static int32 Spdlogger_LogTable(lua_State *L)
{
	int nParamNum = lua_gettop(L);
	if(nParamNum != 4)
	{
		UE_LOG(LogCore,Warning,TEXT("[log]log lua table,pararmnum error %d"), nParamNum);
		return 0;
	}
	if(lua_type(L,1) != LUA_TUSERDATA || lua_type(L,2) != LUA_TNUMBER ||
		lua_type(L,3) != LUA_TSTRING ||  lua_type(L,4) != LUA_TTABLE)
	{
		UE_LOG(LogCore,Warning,TEXT("[log]log lua table,pararmtype error %d"));
		return 0;
	}
	USpdlogger * pLogger = (USpdlogger * )UnLua::GetUObject(L,1);	
	if(pLogger == nullptr)
	{
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("", L);
		return 0;
	}
	FString strModule(UTF8_TO_TCHAR(lua_tostring(L,3)));
	FString strJson;
	if(!FLuaTableToJson::ConvertLuaTableToJson((Table*)lua_topointer(L,4), strJson))
	{
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback("", L);
		return 0;
	}
	
	pLogger->LogJson(lua_tointeger(L,2), strModule, strJson);
	return 0;
}

static const luaL_Reg USpdloggerLib[] =
{
	{ "__call", Spdlogger_New },
	{"LogTable",Spdlogger_LogTable},
	{ nullptr, nullptr }
};

BEGIN_EXPORT_REFLECTED_CLASS(USpdlogger)
    ADD_LIB(USpdloggerLib)
END_EXPORT_CLASS()
IMPLEMENT_EXPORTED_CLASS(USpdlogger)