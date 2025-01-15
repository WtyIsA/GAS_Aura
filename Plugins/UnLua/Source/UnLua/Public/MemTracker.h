#pragma once

namespace UnLua
{
	enum  ELuaLLMTag
	{
#if !UE_BUILD_SHIPPING
		LuaTag = (int)ELLMTag::ProjectTagStart + 10,
#else
		LuaTag = 160,
#endif
		
		LuaDbTag,
		CfgTag,
		MsgTag,
	};

	class UNLUA_API CLMemTracker
	{
	public:
		static void Init();

	private:
		static  bool m_bInit;
	};

	
}

#define MemTrackerScope(tag) LLM_SCOPE((ELLMTag)(tag))