#include "MemTracker.h"

#include "HAL/LowLevelMemStats.h"

#if !UE_BUILD_SHIPPING
DECLARE_LLM_MEMORY_STAT(TEXT("LuaCfg"), STAT_LuaCfgLLM, STATGROUP_LLMFULL);
DECLARE_LLM_MEMORY_STAT(TEXT("LuaMsg"), STAT_LuaMsgLLM, STATGROUP_LLMFULL);
DECLARE_LLM_MEMORY_STAT(TEXT("LuaDB"), STAT_LuaDBLLM, STATGROUP_LLMFULL);
DECLARE_LLM_MEMORY_STAT(TEXT("Lua"), STAT_LuaLLM, STATGROUP_LLMFULL);
#endif

namespace UnLua
{	
	
	bool CLMemTracker::m_bInit = false;
	void CLMemTracker::Init()
	{
#if !UE_BUILD_SHIPPING
		if(!m_bInit && FLowLevelMemTracker::IsEnabled())
		{
			LLM(FLowLevelMemTracker::Get().RegisterProjectTag(ELuaLLMTag::LuaTag, TEXT("Lua"), GET_STATFNAME(STAT_LuaLLM), NAME_None));
			LLM(FLowLevelMemTracker::Get().RegisterProjectTag(ELuaLLMTag::LuaDbTag, TEXT("LuaDB"), GET_STATFNAME(STAT_LuaDBLLM), NAME_None));
			LLM(FLowLevelMemTracker::Get().RegisterProjectTag(ELuaLLMTag::CfgTag, TEXT("LuaCfg"), GET_STATFNAME(STAT_LuaCfgLLM), NAME_None));
			LLM(FLowLevelMemTracker::Get().RegisterProjectTag(ELuaLLMTag::MsgTag, TEXT("LuaMsg"), GET_STATFNAME(STAT_LuaMsgLLM), NAME_None));
			m_bInit = true;
		}
#endif
		
	}
	
}
