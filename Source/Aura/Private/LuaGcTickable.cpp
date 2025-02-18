#include "LuaGcTickable.h"

#include "lgc.h"
#include "lua.h"
#include "LuaManger.h"
#include "UnLuaBase.h"
#include "Aura/AuraLogChannels.h"
#include "Stats/Stats2.h"

DECLARE_STATS_GROUP(TEXT("Lua"), STATGROUP_Lua, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("LuaGc"), STAT_LUAGC, STATGROUP_Lua)


static int GLuaGcPeriod = 120;
static FAutoConsoleVariableRef CVarLuaGcPeriod(
	TEXT("Lua.LuaGcPeriod"), 
	GLuaGcPeriod, 
	TEXT("Lua gc period in seconds"),
	ECVF_Default);

static int GLuaGcFrameTime = 20;
static FAutoConsoleVariableRef CVarLuaGcFrameTime(
	TEXT("Lua.LuaGcFrameTime"), 
	GLuaGcFrameTime, 
	TEXT("Lua gc max time in milliseconds one frame"),
	ECVF_Default);

static int GLuaGcAwakeTime = 5;
static FAutoConsoleVariableRef CVarLuaGcAwakeTime(
	TEXT("Lua.LuaGcAwakeTime"), 
	GLuaGcAwakeTime, 
	TEXT("Lua gc awake time in milliseconds"),
	ECVF_Default);

static bool GLuaGcDebugLog = false;
static FAutoConsoleVariableRef CVarLuaGcDebugLog(
	TEXT("Lua.LuaGcDebugLog"), 
	GLuaGcDebugLog, 
	TEXT("Lua gc debug log switch"),
	ECVF_Default);

FLuaGcTickable::FLuaGcTickable()
{
	LuaState = UnLua::GetState();
	lua_gc(LuaState, LUA_GCSTOP);
	lua_gc(LuaState,LUA_GCINC,0,100,0);
	NextGcTime = FPlatformTime::Seconds() + GLuaGcPeriod;
	GcOnceTime = 0;
}

void FLuaGcTickable::Tick(float DeltaTime)
{
	if (GLuaGcPeriod <= 0)
		return;
	
	global_State *g = G(LuaState);
	if (g->gcstate == GCSpause)
	{
		const double time = FPlatformTime::Seconds();
		if (time < NextGcTime)
			return;
		
		NextGcTime = time + GLuaGcPeriod;
		GcOnceTime = 0;
		UE_LOG(LogAura, Display, TEXT("[Lua]gc begin:mem=%dkb"), lua_gc(LuaState, LUA_GCCOUNT));
	}
	
	double gcTotalTime = 0.0, gcUseTime = 0.0;
	do
	{
		const double gcStepStart = FPlatformTime::Seconds();
		
		{
			SCOPE_CYCLE_COUNTER(STAT_LUAGC)
			lua_gc(LuaState, LUA_GCSTEP, 0);
		}
	
		double gcStepTime = FPlatformTime::Seconds() - gcStepStart;

		if (GLuaGcDebugLog)
		{
			UE_LOG(LogAura, Display, TEXT("[Lua]gc step usetime=%.2fms"), gcStepTime * 1000);
		}
		
		gcUseTime += gcStepTime;
		gcTotalTime += gcStepTime;
		GcOnceTime += gcStepTime;

		if (gcUseTime >= (GLuaGcAwakeTime * 0.001))
		{
			gcUseTime = 0.0;
			//FCoreDelegates::OnGameThreadWakeup.Broadcast();
		}
	} while(g->gcstate != GCSpause && gcTotalTime < (GLuaGcFrameTime * 0.001));

	if (g->gcstate == GCSpause)
	{
		UE_LOG(LogAura, Display, TEXT("[Lua]gc end:mem=%dkb,usedtime=%.2fms"), lua_gc(LuaState, LUA_GCCOUNT), GcOnceTime * 1000);
		GcOnceTime = 0.0;
	}
}
