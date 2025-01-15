
#include "CfgInterface.h"


#include "CfgManager.h"
#include "lua.hpp"
#include "CfgTable.h"
#include "EndianOpt.h"
#include "lapi.h"
#include "LuaManger.h"
#include "LuaMessageTableOptInC.h"
#include "MemTracker.h"
#include "UnLuaEx.h"

void FLuaCfgInterface::InitCfgInfos(FString Info)
{
}

int RegisBean(lua_State *L)
{
	MemTrackerScope(UnLua::ELuaLLMTag::CfgTag);
	int nParamNum = lua_gettop(L);
	if(nParamNum != 1)
	{
		UE_LOG(LogTemp,Error,TEXT("[cfg]RegisBean error param  num=%d"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	int32 Type = lua_type(L, 1);
	if(Type != LUA_TSTRING)
	{
		UE_LOG(LogTemp,Error,TEXT("[cfg]RegisBean error param1(%d) not string"),Type);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	const char* pStr = lua_tostring(L, 1);
	FString strInfo(pStr);	
	if(!FCfgManager::Get()->RegisBean(strInfo))
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
	return 0;
}

int RegisConstTable(lua_State *L)
{
	MemTrackerScope(UnLua::ELuaLLMTag::CfgTag);
	int nParamNum = lua_gettop(L);
	if(nParamNum != 1)
	{
		UE_LOG(LogTemp,Error,TEXT("RegisBean error param  num=%d"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	int32 Type = lua_type(L, 1);
	if(Type != LUA_TSTRING)
	{
		UE_LOG(LogTemp,Error,TEXT("[cfg]RegisBean error param1(%d) not string"),Type);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	const char* pStr = lua_tostring(L, 1);
	FString strInfo(pStr);
	if(FCfgManager::Get()->RegisConstTable(strInfo) != nullptr)
		return 1;
	CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
	return 0;
}

int RegisTable(lua_State *L)
{
	MemTrackerScope(UnLua::ELuaLLMTag::CfgTag);
	int nParamNum = lua_gettop(L);
	if(nParamNum != 1)
	{
		UE_LOG(LogTemp,Error,TEXT("[cfg]RegisTable error param  num=%d"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	int32 Type = lua_type(L, 1);
	if(Type != LUA_TSTRING)
	{
		UE_LOG(LogTemp,Error,TEXT("[cfg]RegisTable error param1(%d) not string"),Type);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	const char* pStr = lua_tostring(L, 1);
	FString strInfo(pStr);
	if(FCfgManager::Get()->RegisTable(strInfo) != nullptr)
		return  1;
	return  CLuaMessageTableOptInC::PrintLuaTrackbackAndPushNil(nullptr,L);
}

int GetLuaCfgTableLen(lua_State*L)
{
	int nParamNum = lua_gettop(L);
	if(nParamNum < 1 )
	{
		UE_LOG(LogTemp,Error,TEXT("[cfg]LuaCfgGetLen error param  num=%d"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	int nType = lua_type(L,1);
	if(nType != LUA_TTABLE)
	{
		UE_LOG(LogTemp,Error,TEXT("[cfg]GetLuaCfgTableLen  param(%d) should be table"),nType);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		lua_pushinteger(L,0);
		return 1;
	}
	const void* p = lua_topointer(L,1);
	Table* pTable = (Table*)p;
	FCfgTable* pCTab = FCfgManager::Get()->GetCfgTable(pTable);
	if(pCTab != nullptr)
	{
		lua_pushinteger(L, pCTab->GetRowsCount());
		return 1;
	}
	lua_pushinteger(L,0);
	CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
	return 1;
}

int LuaCfgTable_Index(lua_State*L)
{
	MemTrackerScope(UnLua::ELuaLLMTag::CfgTag);
	int nParamNum = lua_gettop(L);
	if(nParamNum != 2 )
	{
		UE_LOG(LogTemp,Error,TEXT("[cfg]LuaCfgTable_Index error param  num=%d"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback(nullptr, L);
		return 0;
	}
	
	const void* p = lua_topointer(L,1);
	Table* pTable = (Table*)p;
	FCfgTable* pCTab = FCfgManager::Get()->GetCfgTable(pTable);
	if(pCTab == nullptr)
	{
		UE_LOG(LogTemp,Error,TEXT("[cfg]LuaCfgTable_Index error table not exist"));
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback(nullptr, L);
		return 0;
	}
	int nType = lua_type(L,2);	
	uint8* pRow = nullptr;
	if(nType == LUA_TNUMBER)
	{		
		pRow = pCTab->GetRow(lua_tointeger(L,2));		
	}	
	else if(nType == LUA_TSTRING)
	{
		FString strKey(UTF8_TO_TCHAR(lua_tostring(L,2)));
		pRow  = pCTab->GetRow(strKey);
	}
	else
	{
		return 0;
	}
	if(pRow != nullptr)
	{				
		SCfgItemRow* pItemRom = (SCfgItemRow*)pRow;		
        if(FCfgTable::PushItemRowToLua(pItemRom, __CfgItemMetaName__,L))
        	return 1; 
	}
	
	return 0;
}

int LuaCfgTable_NewIndex(lua_State*L)
{
	int nParamNum = lua_gettop(L);
	if(nParamNum != 3)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]LuaCfgGet_NewIndex param error %d"), nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	lua_rawset(L,-3);
	return 0;
}

//迭代器，在x.lua里的_next(t,index)里调用
int IterLuaTable(lua_State *L)
{
	MemTrackerScope(UnLua::ELuaLLMTag::CfgTag);
	int nParamNum = lua_gettop(L);
	if(nParamNum != 2)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]IterLuaTable error params  num(%d) !=2"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	const void* p = lua_topointer(L,1);
	//FString name( lua_tostring(L,1));
	FCfgTable* pcfgTable = FCfgManager::Get()->GetCfgTable((Table*)p);
	if(pcfgTable == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]IterLuaTable error name not exist %p"),p);
		return  CLuaMessageTableOptInC::PrintLuaTrackbackAndPushNil(nullptr,L);
	}
	int32 Type = lua_type(L, 2);
	if(Type == LUA_TNIL)
	{
		return pcfgTable->IterTable(L);
	}	
	if(Type == LUA_TNUMBER)
	{
		int64 nId = lua_tointeger(L, 2);
		return pcfgTable->IterTable(&nId,L);
	}
	else if(Type == LUA_TSTRING)
	{
		return pcfgTable->IterTable(lua_tostring(L,2),L);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]IterLuaTable  from  key type error %d"), Type);
		return  CLuaMessageTableOptInC::PrintLuaTrackbackAndPushNil(nullptr,L);
	}	
	return 0;
}


int LuaCfgTable_pairs(lua_State* L)
{
	MemTrackerScope(UnLua::ELuaLLMTag::CfgTag);
	int nParamNum = lua_gettop(L);
	if(nParamNum != 1)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]LuaCfgTable_pairs param error %d"), nParamNum);
		return  CLuaMessageTableOptInC::PrintLuaTrackbackAndPushNil(nullptr,L);
	}
	if(lua_type(L,1) == LUA_TTABLE)
	{
		const void* p = lua_topointer(L,1);
		Table* pTable = (Table*)p;
		FCfgTable* pCTab = FCfgManager::Get()->GetCfgTable(pTable);
		if(pCTab != nullptr)
		{
			lua_pushcfunction(L, IterLuaTable);
			CLuaMessageTableOptInC::PushTable(pTable,L);
			lua_pushnil(L);
			return 3;
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("[cfg]LuaCfgTable_pairs param table null"));
			return  CLuaMessageTableOptInC::PrintLuaTrackbackAndPushNil(nullptr,L);
		}
	}
	return 0;
}

int GetCfgRowItem_Index(lua_State *L)
{
	MemTrackerScope(UnLua::ELuaLLMTag::CfgTag);
	int nParamNum = lua_gettop(L);
	if(nParamNum < 2)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetCfgItem_Index error, params num=%d"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	//SCfgItemRow* pRow = (SCfgItemRow*)*(void**)luaL_checkudata(L,1,__CfgItemMetaName__);
	void* pUd =  lua_touserdata(L,1);
	SCfgItemRow* pRow =nullptr;
	if(pUd != nullptr)
		pRow = (SCfgItemRow*)*(void**)pUd;
	if(pRow == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetCfgRowItem_Index ud is null"));
		return  CLuaMessageTableOptInC::PrintLuaTrackbackAndPushNil(nullptr,L);
	}
	int nluaType = lua_type(L, 2);
	if(nluaType != LUA_TSTRING)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetCfgRowItem_Index error, param2(%d) not string" ),nluaType);
		return  CLuaMessageTableOptInC::PrintLuaTrackbackAndPushNil(nullptr,L);
	}
	FCfgTable* pCfgTable = FCfgManager::Get()->GetCfgTable(pRow->m_beanId);
	if(pCfgTable == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetCfgRowItem_Index error, beanId not exist %d"), pRow->m_beanId);
		return  CLuaMessageTableOptInC::PrintLuaTrackbackAndPushNil(nullptr,L);
	}
	FString strItemName( lua_tostring(L,2));
	if(pCfgTable->GetColumnByName(pRow, strItemName, L))
	{
		return 1;
	}
	lua_pushnil(L);
	return 1;
	
}

int GetCfgBeanItem_Index(lua_State *L)
{
	MemTrackerScope(UnLua::ELuaLLMTag::CfgTag);
	int nParamNum = lua_gettop(L);
	if(nParamNum < 2)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetCfgBeanItem_Index error, params num=%d"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	void* pUd =  lua_touserdata(L,1);
	SCfgItemRow* pRow =nullptr;
	if(pUd != nullptr)
		pRow = (SCfgItemRow*)*(void**)pUd;
	if(pRow == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetCfgBeanItem_Index usedata null num=%d"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return  0;
	}
	int nluaType = lua_type(L, 2);
	if(nluaType != LUA_TSTRING)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetCfgBeanItem_Index error, param2(%d) not string" ),nluaType);
		return  CLuaMessageTableOptInC::PrintLuaTrackbackAndPushNil(nullptr,L);
	}	
	FString strItemName( lua_tostring(L,2));
	if(FCfgTable::GetBeanColumnByName(pRow, strItemName,L))
	{
		return 1;
	}
	lua_pushnil(L);
	return 1;
}

int GetListItem(lua_State *L)
{
	MemTrackerScope(UnLua::ELuaLLMTag::CfgTag);
	int nParamNum = lua_gettop(L);
	if(nParamNum != 2)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetListItem error, params num=%d"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	void* pUd =  lua_touserdata(L,1);
	SCfgItemRow* pRow =nullptr;
	if(pUd != nullptr)
		pRow = (SCfgItemRow*)*(void**)pUd;
		
	if(pRow == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetListItem usedata is null"));
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return  0;
	}
	int nluaType = lua_type(L, 2);
	if(nluaType != LUA_TNUMBER)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetListItem error, param2(%d) not int"), nluaType);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	if(FCfgTable::GetListSubItem(pRow, lua_tointeger(L,2),L ))
		return 1;
	return 0;
}

int GetListSetMapSize(lua_State* L ,const char* metaName)
{
	MemTrackerScope(UnLua::ELuaLLMTag::CfgTag);
	int nParamNum = lua_gettop(L);
	if(nParamNum != 1)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetListSetMapSize error, params num=%d"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	//SCfgItemRow* pRow = (SCfgItemRow*)*(void**)luaL_checkudata(L,1,metaName);
	void* pUd =  lua_touserdata(L,1);
	SCfgItemRow* pRow =nullptr;
	if(pUd != nullptr)
		pRow = (SCfgItemRow*)*(void**)pUd;
	if(pRow == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetListSetMapSize row error=%d"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return  0;
	}
	int nSize = FCfgTable::GetListSize(pRow);
	lua_pushinteger(L,nSize);
	return 1;
}

int GetListSize(lua_State *L)
{
	return  GetListSetMapSize(L, __CfgListMetaName__);	
}

int GetSetSize(lua_State *L)
{
	return  GetListSetMapSize(L, __CfgSetMetaName__);
}

int CheckSetContain(lua_State *L)
{
	int nParamNum = lua_gettop(L);
	if(nParamNum != 2)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]CheckSetContain error, params num=%d"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	//SCfgItemRow* pRow = (SCfgItemRow*)*(void**)luaL_checkudata(L,1,__CfgSetMetaName__);
	void* pUd =  lua_touserdata(L,1);
	SCfgItemRow* pRow =nullptr;
	if(pUd != nullptr)
		pRow = (SCfgItemRow*)*(void**)pUd;
	if(pRow == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]CheckSetContain usedata null"));
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return  0;
	}
	int32 Type = lua_type(L, 2);
	if(Type == LUA_TNUMBER)
	{
		uint64 nId = lua_tointeger(L, 2);
		lua_pushboolean(L, FCfgTable::CheckSetContain(pRow, nId) );
		return 1;
	}
	else if(Type == LUA_TSTRING)
	{
		lua_pushboolean(L, FCfgTable::CheckSetContain(pRow, lua_tostring(L,2)) );
		return  1;
	}
	return 0;
}

int GetMapSize(lua_State *L)
{
	return  GetListSetMapSize(L, __CfgMapMetaName__);
}

int GetMapItem(lua_State *L)
{
	MemTrackerScope(UnLua::ELuaLLMTag::CfgTag);
	int nParamNum = lua_gettop(L);
	if(nParamNum != 2)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetMapItem error, params num=%d"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	//SCfgMapRow* pRow = (SCfgMapRow*)*(void**)luaL_checkudata(L,1,__CfgMapMetaName__);
	void* pUd =  lua_touserdata(L,1);
	SCfgMapRow* pRow =nullptr;
	if(pUd != nullptr)
		pRow = (SCfgMapRow*)*(void**)pUd;
	if(pRow == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetMapItem usedata null"));
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return  0;
	}
	int nluaType = lua_type(L, 2);
	if(nluaType == LUA_TNUMBER)
	{
		uint64 nId = lua_tointeger(L, 2);
		if(FCfgTable::GetMapItemByKey(pRow, nId,L))
			return 1;
	}
	else if(nluaType == LUA_TSTRING)
	{
		if(FCfgTable::GetMapItemByKey(pRow, lua_tostring(L,2),L))
			return  1;
	}
	
	return 0;
}
int GetMapItemByIndex(lua_State *L)
{
	MemTrackerScope(UnLua::ELuaLLMTag::CfgTag);
	int nParamNum = lua_gettop(L);
	if(nParamNum != 2)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetMapItemByIndex error, params num=%d"),nParamNum);
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return 0;
	}
	//SCfgMapRow* pRow = (SCfgMapRow*)*(void**)luaL_checkudata(L,1,__CfgMapMetaName__);
	void* pUd =  lua_touserdata(L,1);
	SCfgMapRow* pRow =nullptr;
	if(pUd != nullptr)
		pRow = (SCfgMapRow*)*(void**)pUd;
	if(pRow == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]GetMapItemByIndex usedata null"));
		CLuaMessageTableOptInC::OnlyPrintLuaTrackback();
		return  0;
	}
	int nluaType = lua_type(L, 2);
	if(nluaType == LUA_TNUMBER)
	{
		uint32 nId = lua_tointeger(L, 2);
		if(FCfgTable::GetMapItemByIndex(pRow, nId,L))
			return 1;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[cfg]GetMapItemByIndex param type(%d) error not number"), nluaType);
	}
	

	return 0;
}


int Cfg_Gc(lua_State *L, const char * metaName)
{
	int nParamNum = lua_gettop(L);
	if(nParamNum != 1)
	{
		return 0;
	}
	//SCfgItemRow* pRow = (SCfgItemRow*)*(void**)luaL_checkudata(L,1,metaName);
	void* pUd =  lua_touserdata(L,1);
	SCfgItemRow* pRow =nullptr;
	if(pUd != nullptr)
		pRow = (SCfgItemRow*)*(void**)pUd;
	if(pRow == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("[cfg]Cfg_Gc errer,not SCfgItemRow"));
		return  0;
	}
	pRow->m_luaUd = nullptr;
	
	return 0;
}

int CfgRowItem_Gc(lua_State *L)
{
	return Cfg_Gc(L, __CfgItemMetaName__);	
}

int CfgBeanItem_Gc(lua_State *L)
{	
	return Cfg_Gc(L, __CfgBeanMetaName__);	
}


int CfgListItem_Gc(lua_State *L)
{
	return Cfg_Gc(L, __CfgListMetaName__);	
}

int CfgSetItem_Gc(lua_State *L)
{
	return Cfg_Gc(L, __CfgSetMetaName__);	
}

int CfgMapItem_Gc(lua_State *L)
{
	return Cfg_Gc(L, __CfgMapMetaName__);	
}


static const luaL_Reg FLuaCfgListItemLib[] =
{
	{"get", GetListItem},
	{"size", GetListSize},
	{"__gc", CfgListItem_Gc},
	{ nullptr, nullptr }
};

EXPORT_UNTYPED_CLASS(FLuaCfgListItem, false, FLuaCfgListItemLib)
IMPLEMENT_EXPORTED_CLASS(FLuaCfgListItem)

static const luaL_Reg FLuaCfgSetItemLib[] =
{
	{"contains", CheckSetContain},
{"size", GetSetSize},
{"__gc", CfgSetItem_Gc},
{ nullptr, nullptr }
};

EXPORT_UNTYPED_CLASS(FLuaCfgSetItem, false, FLuaCfgSetItemLib)
IMPLEMENT_EXPORTED_CLASS(FLuaCfgSetItem)

static const luaL_Reg FLuaCfgMapItemLib[] =
{
	{"get", GetMapItem},
	{"getAt", GetMapItemByIndex},
{"size", GetMapSize},
{"__gc", CfgMapItem_Gc},
{ nullptr, nullptr }
};

EXPORT_UNTYPED_CLASS(FLuaCfgMapItem, false, FLuaCfgMapItemLib)
IMPLEMENT_EXPORTED_CLASS(FLuaCfgMapItem)

static const luaL_Reg FLuaCfgBeanItemLib[] =
{
	{"__index",GetCfgBeanItem_Index},
{"__gc", CfgBeanItem_Gc},
{ nullptr, nullptr }
};

EXPORT_UNTYPED_CLASS(FLuaCfgBeanItem, false, FLuaCfgBeanItemLib)
IMPLEMENT_EXPORTED_CLASS(FLuaCfgBeanItem)

static const luaL_Reg FLuaCfgItemLib[] =
{
	{"__index",GetCfgRowItem_Index},
	{"__gc", CfgRowItem_Gc},
{ nullptr, nullptr }
};

EXPORT_UNTYPED_CLASS(FLuaCfgItem, false, FLuaCfgItemLib)
IMPLEMENT_EXPORTED_CLASS(FLuaCfgItem)

static const luaL_Reg FLuaCfgInterfaceLib[] =
{
	{"RegisTable", RegisTable},
	{"RegisBean",RegisBean},
	{"RegisConstTable",RegisConstTable},
	{"__len", GetLuaCfgTableLen},
{"__index", LuaCfgTable_Index},
	{"__newindex", LuaCfgTable_NewIndex},
	{"__pairs", LuaCfgTable_pairs},
	{ nullptr, nullptr }
};

BEGIN_EXPORT_CLASS(FLuaCfgInterface)

ADD_FUNCTION(InitCfgInfos)
ADD_LIB(FLuaCfgInterfaceLib)
END_EXPORT_CLASS()
IMPLEMENT_EXPORTED_CLASS(FLuaCfgInterface)

