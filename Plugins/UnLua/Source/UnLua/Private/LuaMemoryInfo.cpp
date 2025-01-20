#include "LuaMemoryInfo.h"

//#include "DelegateHelper.h"
#include "UnLuaBase.h"
#include "lua.hpp"
#include "lualib.h"
#include "ltable.h"
#include "lstate.h"
#include "lobject.h"
#include "lapi.h"
#include "lgc.h"
#include "UnLuaBase.h"
#include "LuaMessageTableOptInC.h"
#include "ObjectReferencer.h"
#include "UnLuaEx.h"
#include "UnLuaModule.h"

FLuaMemoryInfo::~FLuaMemoryInfo()
{
	for(auto&item:LuaObjRefInfo)
	{
		for(auto& objRefInfo:item.Value)
		{
			delete objRefInfo;
		}
		item.Value.Empty();
	}
	LuaObjRefInfo.Empty();
}


FString FLuaMemoryInfo::GetNameFromNode(Node* pNode)
{
	if(pNode == nullptr)
		return FString();
	FString strKey;
	TValue pTempValue;
	CLuaMessageTableOptInC::GetKeyFromNode(pNode, pTempValue);

	if (ttisinteger(&pTempValue))
		strKey = FString::Printf(TEXT("%lld"), ivalue(&pTempValue));
	else if (ttisnumber(&pTempValue))
		strKey = FString::Printf(TEXT("%f"), fltvalue(&pTempValue));
	else if (ttisstring(&pTempValue))
		strKey = UTF8_TO_TCHAR(svalue(&pTempValue));
	else
	{
		strKey = UTF8_TO_TCHAR(lua_typename(UnLua::GetState(),ttype(&pTempValue)));
	}
	return strKey;
}


void FLuaMemoryInfo::GetGlobalTables()
{
	lua_State* L = UnLua::GetState();
	Table* reg = hvalue(&G(L)->l_registry);
	const TValue* global;
	lua_lock(L);
	global = luaH_getint(reg, LUA_RIDX_GLOBALS);
	lua_unlock(L);
	Table *GlobalPointer = hvalue(global);
	unsigned int nCount = sizenode(GlobalPointer);
	for(unsigned int i = 0; i < nCount; i++)
	{
		Node* pNode = gnode(GlobalPointer, i);
		if(pNode == nullptr)
			continue;        
		if(&pNode->u.key_tt == LUA_TNIL || ttisnil(&pNode->i_val))
		{
			continue;
		}
		
		FString strKey = GetNameFromNode(pNode);
		if(ttistable(&pNode->i_val)){
			GlobalTableToName.Add(hvalue(&pNode->i_val), strKey);
		}
		if(ttisfulluserdata(&pNode->i_val)){
			GlobalTableToName.Add(uvalue(&pNode->i_val), strKey);
		}
	}
	nCount = sizenode(reg);
	for (unsigned int i = 0; i < nCount; i++)
	{
		Node* pNode = gnode(reg, i);
		if (pNode == nullptr)
			continue;
		if (&pNode->u.key_tt == LUA_TNIL || ttisnil(&pNode->i_val))
		{
			continue;
		}

		FString strKey = GetNameFromNode(pNode);
		if(ttistable(&pNode->i_val)){
			GlobalTableToName.Add(hvalue(&pNode->i_val), strKey);
		}
		if(ttisfulluserdata(&pNode->i_val)){
			GlobalTableToName.Add(uvalue(&pNode->i_val), strKey);
		}
	}
}

void FLuaMemoryInfo::GetAllLoadModule()
{
	lua_State* L = UnLua::GetState();
	Table* reg = hvalue(&G(L)->l_registry);
	Table* GlobalPointer = CLuaMessageTableOptInC::GetTable(reg, LUA_LOADED_TABLE, L);
	unsigned int nCount = sizenode(GlobalPointer);
	for (unsigned int i = 0; i < nCount; i++)
	{
		Node* pNode = gnode(GlobalPointer, i);
		if (isempty(gval(pNode)))
			continue;
		FString strKey = GetNameFromNode(pNode);
		if (ttistable(&pNode->i_val))
		{
			LoadModuleTableToName.Add(hvalue(&pNode->i_val), strKey);
		}
		else
		{
			LoadModuleTableToName.Add(&pNode->i_val, strKey);
		}
	}
}

void FLuaMemoryInfo::AnalyzeGcObj()
{
	GetGlobalTables();
	GetAllLoadModule();
	lua_State *L = UnLua::GetState();
	int top = lua_gettop(L);
	GCObject *pGcObj = G(L)->allgc;
	AnalyzeGcObj(pGcObj);
	pGcObj = G(L)->finobj;
	AnalyzeGcObj(pGcObj);
	lua_settop(L,top);
}

void FLuaMemoryInfo::AnalyzeGcObj(GCObject* gcHeader)
{
	while (gcHeader != NULL)
	{
		if (gcHeader->tt == LUA_TTABLE)
		{
			AnalyzeObj(gco2t(gcHeader));
		}
		// else if(gcHeader->tt == LUA_TSTRING)
		// {		
		// 	LuaAllTStrings.Add(gco2ts(gcHeader));
		// }
		else if(gcHeader->tt  == LUA_VUSERDATA)
			AnalyzeObj(gco2u(gcHeader));
		//else if(gcHeader->tt == LUA_VLCL)
		//	AnalyzeObj(gco2lcl(gcHeader), gcHeader->marked);
		gcHeader = gcHeader->next;
	}
}


void FLuaMemoryInfo::AnalyzeObj(Udata* pUseData)
{
	if(pUseData == nullptr)
		return;
	if(pUseData->metatable)
	{
		auto& item = MetaTableUseTimes.FindOrAdd(pUseData->metatable);
        item.nTimes++;
		int nSize = sizeudata(0, pUseData->len);		
		if(item.nMaxSize < nSize)
			item.nMaxSize = nSize;
		item.nTotalSize += nSize;
	}
}

void FLuaMemoryInfo::AnalyzeObj(Table* pTable)
{
	if(pTable == nullptr)
    		return;

	if (pTable->metatable)
	{
		auto& item = MetaTableUseTimes.FindOrAdd(pTable->metatable);
		item.nTimes++;
		int nSize = sizeof(Table) + 8*pTable->alimit + 8*sizenode(pTable);		
		if(item.nMaxSize < nSize)
			item.nMaxSize = nSize;
		item.nTotalSize += nSize;
	}    	
}

void FLuaMemoryInfo::AnalyzeObj(LClosure* plClosure,int state)
{
	
	lua_State *L = UnLua::GetState();
	LClosure *cl = plClosure;
	lua_lock(L);
	setclLvalue(L, &L->top->val, cl);
	api_incr_top(L);
	lua_unlock(L);

	lua_Debug ar;
			
	lua_pushvalue(L, -1);
			
	lua_getinfo(L, ">Sl", &ar);

	
	lua_pop(L, 1);

	FString strSource = FString::Printf(TEXT("%s_%d"),UTF8_TO_TCHAR(ar.short_src), ar.linedefined);
	if(FunToName.Contains(plClosure))
	{
		UE_LOG(LogTemp,Display,TEXT("lua function repert %p  , %s"), plClosure,*strSource);
		return;
	}
	FunToName.Add(plClosure, strSource);
	auto& item = LuaFunctionUserTimes.FindOrAdd(strSource);
	item.Add(cl);
}

FString FLuaMemoryInfo::GetTableName(Table* obj)
{
	FString strName;
	FString* pName = GlobalTableToName.Find(obj);
	if(pName)
	{
		return FString::Printf(TEXT("%p:%s"), obj,**pName);
	}
	
	const TValue* p = CLuaMessageTableOptInC::GetTValue(obj,"__name");
	if(p && ttisstring(p))
	{
		FString tempName(UTF8_TO_TCHAR(svalue(p)));
		return FString::Printf(TEXT("%p_%s"), obj,*tempName);
	}

	if(obj->metatable)
	{
		pName = GlobalTableToName.Find(obj->metatable);
		if(pName)
			return FString::Printf(TEXT("%p:%s"), obj,**pName);
	}
	
	return FString::Printf(TEXT("table_%p"), obj);
}

void FLuaMemoryInfo::SaveResult(FString savePath)
{
	int nFileId = 0;
	FString tempFilePath = FPaths::GetPath(savePath);
	FString strResult;
	if(!IFileManager::Get().DirectoryExists(*tempFilePath))
	{
		IFileManager::Get().MakeDirectory(*tempFilePath, true);
	}
	
	tempFilePath = savePath;
	while (FPaths::FileExists(tempFilePath))
	{
		nFileId++;
		tempFilePath = FString::Printf(TEXT("%s_%d.%s"), *FPaths::GetBaseFilename(savePath,false), nFileId, *FPaths::GetExtension(savePath));
	}
	/*
	for(auto item: LuaFunctionUserTimes)
	{
		TArray<void*>& funs = item.Value;
		strResult.Appendf(TEXT("\r\n%s,%d"), *item.Key, funs.Num());
		for(int i = 0; i < funs.Num(); i++)
			strResult.Appendf(TEXT(",%p"), funs[i]);		
	}	
	FFileHelper::SaveStringToFile(strResult, *tempFilePath);
	*/
	strResult.Empty();
	for(auto item: MetaTableUseTimes)
	{
		FString* metaName = GlobalTableToName.Find(item.Key);
		FLuaTableMemInfo& memInfo = item.Value;
		if(metaName)
		{
			strResult.Appendf(TEXT("%s,%d,%d,%d\r\n"), **metaName, memInfo.nTimes,memInfo.nMaxSize,memInfo.nTotalSize);			
		}
		else
		{
			const TValue* p = CLuaMessageTableOptInC::GetTValue(item.Key,"__name");
			if(p && ttisstring(p))
			{
				FString tempName(UTF8_TO_TCHAR(svalue(p)));
				strResult.Appendf(TEXT("%s,%d,%d,%d\r\n"), *tempName, memInfo.nTimes,memInfo.nMaxSize,memInfo.nTotalSize);
			}
			else
				strResult.Appendf(TEXT("table:%p,%d,%d,%d\r\n"), item.Key, memInfo.nTimes,memInfo.nMaxSize,memInfo.nTotalSize);
		}
	}
	strResult.Appendf(TEXT("totalMem,1,1,%lld"),gettotalbytes(G(UnLua::GetState())));
	//tempFilePath = FString::Printf(TEXT("%s_%d.csv"), *savePath, nFileId);
	FFileHelper::SaveStringToFile(strResult, *tempFilePath);

	/*
	strResult.Empty();
	
	for(auto&item:FDelegateHelper::GetAllCallback())
	{
		strResult.Appendf(TEXT("%p,%s,%p\r\n"), item.Value, *FDelegateHelper::GetBindedFunctionName(item.Key).ToString(), item.Key.Object);
	}
	
	tempFilePath = FString::Printf(TEXT("%s_delegate_%d.csv"), *savePath, nFileId);
	FFileHelper::SaveStringToFile(strResult, *tempFilePath);
	*/
}

void FLuaMemoryInfo::GetObjRelation()
{
	lua_State *L = UnLua::GetState();
	int top = lua_gettop(L);
	GetGlobalTables();
	Table* reg = hvalue(&G(L)->l_registry);
	
	GetObjRelation(reg, nullptr);
	
	lua_settop(L,top);
}

void FLuaMemoryInfo::GetObjRelation(FString strPanelName)
{
	StrUIPanelName = strPanelName;
	GetObjRelation();
}


void FLuaMemoryInfo::GetObjRelation(TValue* pTempValue, Table* pParentTable, RelationshipType type, const char* key, int numKey, const char* key2, FString tableName)
{	
	if(ttistable(pTempValue))
	{
		Table*child = hvalue(pTempValue);
		ObjectRelationshipReport(pParentTable, child, type, key, numKey, key2,tableName);
		GetObjRelation(child, pParentTable);
	}
	else if(ttisfulluserdata(pTempValue)){
		Udata* child =uvalue(pTempValue);
		ObjectRelationshipReport(pParentTable, child, type, key, numKey, key2,tableName);
		GetObjRelation(child);
	}
	else if(ttisclosure(pTempValue))
	{
		LClosure* cl =clLvalue(pTempValue);
		ObjectRelationshipReport(pParentTable, cl, type, key, numKey, key2,tableName);
		GetObjRelation(cl, pParentTable);
	}
	else if(ttisCclosure(pTempValue))
	{
		CClosure * cc = clCvalue(pTempValue);
		ObjectRelationshipReport(pParentTable, cc,  type, key, numKey, key2,tableName);
		GetObjRelation(cc);
	}
}


void FLuaMemoryInfo::GetObjRelation(Table* pTable, void* pParent)
{
	if(HadAnalyzeObj.Contains(pTable))
		return;
	HadAnalyzeObj.Add(pTable);
	unsigned int nCount = pTable->alimit;
	FString tableName = GetTableName(pTable);
	if(pTable->metatable)
	{
		ObjectRelationshipReport(pTable, pTable->metatable, Relationship_Metatable, nullptr,0,nullptr,tableName);
		GetObjRelation(pTable->metatable, pTable);
	}
	for(unsigned int i = 0; i < nCount; i++)
	{
		TValue* pTempValue = &pTable->array[i];
		if(ttisnil(pTempValue))
			continue;
		
		GetObjRelation(pTempValue, pTable,Relationship_Key_Number, nullptr, i, nullptr, tableName );
	}

	nCount = sizenode(pTable);
	FString strKey;
	for(unsigned int i = 0; i < nCount; i++)
	{
		Node* pNode = gnode(pTable, i);
		if(pNode == nullptr)
			continue;        
		if(&pNode->u.key_tt == LUA_TNIL || ttisnil(&pNode->i_val))
		{
			continue;
		}
		strKey = GetNameFromNode(pNode);

		TValue pTempValue;
		CLuaMessageTableOptInC::GetKeyFromNode(pNode, pTempValue);
		GetObjRelation(&pTempValue, pTable,Relationship_Key_Str, TCHAR_TO_UTF8(*(strKey + TEXT("___inkey___"))), i, nullptr,tableName);
		GetObjRelation(&pNode->i_val, pTable,Relationship_Key_Str, TCHAR_TO_UTF8(*strKey), i, nullptr,tableName);
		/*
		if(ttistable(&pNode->i_val))
		{
			Table*child = hvalue(&pNode->i_val);
			ObjectRelationshipReport(pTable, child, Relationship_Key_Str, TCHAR_TO_UTF8(*strKey), i, nullptr,tableName);
			GetObjRelation(child, pTable);
		}
		else if(ttisfulluserdata(&pNode->i_val)){
			Udata* child =uvalue(&pNode->i_val);
			ObjectRelationshipReport(pTable, child, Relationship_Key_Str, TCHAR_TO_UTF8(*strKey), i, nullptr,tableName);
			GetObjRelation(child);
		}
		else if(ttisclosure(&pNode->i_val))
		{
			LClosure* cl =clLvalue(&pNode->i_val);
			ObjectRelationshipReport(pTable, cl, Relationship_Key_Str, TCHAR_TO_UTF8(*strKey), i, nullptr,tableName);
			GetObjRelation(cl, pTable);
		}
		*/
	}
}

void FLuaMemoryInfo::GetObjRelation(Udata* pUdata)
{
	if(HadAnalyzeObj.Contains(pUdata))
		return;
	HadAnalyzeObj.Add(pUdata);
	if(pUdata->metatable)
	{
		ObjectRelationshipReport(pUdata, pUdata->metatable, Relationship_Metatable, nullptr, 0, nullptr);
		GetObjRelation(pUdata->metatable, pUdata);
		//
	}
}

void FLuaMemoryInfo::GetObjRelation(CClosure* pCClosure)
{
	if(HadAnalyzeObj.Contains(pCClosure))
		return;
	HadAnalyzeObj.Add(pCClosure);

	lua_State *L = UnLua::GetState();
	CClosure *cl = pCClosure;
	lua_lock(L);
	setclCvalue(L, &L->top->val, cl);
	api_incr_top(L);
	lua_unlock(L);
	int i;
	const char *name;
	for (i=1;;i++)
	{
		name = lua_getupvalue(L,-1,i);
		if (name == NULL)
			break;			
		if (*name != '\0')
		{
			const TValue* pTempValue = s2v(L->top - 1);
			int nType = lua_type(L,-1);
			if(nType==LUA_TTABLE)
			{
				Table* pv = (Table*)lua_topointer(L, -1);
				ObjectRelationshipReport(cl, pv, Relationship_UpValue, "lightusedate", -1, name);
				GetObjRelation(pv, cl);                                         			
			}
			else if(ttisfulluserdata(pTempValue))
			{
				Udata* pv =uvalue(pTempValue);
				ObjectRelationshipReport(cl, pv, Relationship_UpValue, "lightusedate", -1, name);
				GetObjRelation(pv);         
			}
			else if(ttisclosure(pTempValue))
			{
				LClosure* clChild =clLvalue(pTempValue);
				ObjectRelationshipReport(cl, clChild, Relationship_UpValue, "lightusedate", -1,  name);
				GetObjRelation(clChild, cl);
			}
			
		}
			
		lua_pop(L, 1);
	}
}


void FLuaMemoryInfo::GetObjRelation(LClosure* plClosure, void* pParent)
{
	if(HadAnalyzeObj.Contains(plClosure))
		return;
	HadAnalyzeObj.Add(plClosure);

	lua_State *L = UnLua::GetState();
	LClosure *cl = plClosure;
	lua_lock(L);
	setclLvalue(L, &L->top->val, cl);
	api_incr_top(L);
	lua_unlock(L);

	lua_Debug ar;
			
	lua_pushvalue(L,-1);		
	lua_getinfo(L, ">Sl", &ar);
	

	FString strSource = FString::Printf(TEXT("%s_%d"),UTF8_TO_TCHAR(ar.short_src), ar.linedefined);
	FunToName.Add(cl, strSource);

	if(ar.linedefined > 0)
	{
		int i;
		const char *name;
		for (i=1;;i++)
		{
			name = lua_getupvalue(L,-1,i);
			if (name == NULL)
				break;			
			if (*name != '\0')
			{
				const TValue* pTempValue = s2v(L->top - 1);
				int nType = lua_type(L,-1);
				if(nType==LUA_TTABLE)
				{
					Table* pv = (Table*)lua_topointer(L, -1);
					ObjectRelationshipReport(cl, pv, Relationship_UpValue, ar.short_src, ar.linedefined, name);
					GetObjRelation(pv, cl);                                         			
				}
				else if(ttisfulluserdata(pTempValue))
				{
					Udata* pv =uvalue(pTempValue);
					ObjectRelationshipReport(cl, pv, Relationship_UpValue, ar.short_src, ar.linedefined, name);
					GetObjRelation(pv);         
				}
				else if(ttisclosure(pTempValue))
				{
					LClosure* clChild =clLvalue(pTempValue);
					ObjectRelationshipReport(cl, clChild, Relationship_UpValue, ar.short_src, ar.linedefined, name);
					GetObjRelation(clChild, cl);
				}
			
			}
			
			lua_pop(L, 1);
		}
	}
	
	lua_pop(L, 1);
}

void FLuaMemoryInfo::ObjectRelationshipReport(void* parent, void* child, RelationshipType type, const char* key, int numKey, const char* key2, FString parentTableName)
{
	if(key2 && strcmp(key2,"_ENV") == 0)
		return;
	lua_State *L = UnLua::GetState();
	Table* reg = hvalue(&G(L)->l_registry);
	bool hasNext = parent != reg;
	TArray<FLuaTableRef*>& list = LuaObjRefInfo.FindOrAdd(child);
	FString keyName;
	switch (type)
	{
	case Relationship_Key_Number:
		keyName = FString::Printf(TEXT("%d"), numKey);
		break;
	case Relationship_Key_Str:
		keyName = UTF8_TO_TCHAR(key);
		break;
	case Relationship_Metatable:
		keyName = TEXT("__metatable");
		break;
	case Relationship_UpValue:
		hasNext = false;
		keyName = FString::Printf(TEXT("%s:local_%s_line[%d]_%p"), UTF8_TO_TCHAR(key), UTF8_TO_TCHAR(key2),numKey,parent);
		break;
	default:
		keyName = TEXT("UNknow_KEY");
		break;		
	}
	if(!hasNext)
		keyName = TEXT("_R.") + keyName;
	else
	{
		keyName = parentTableName + TEXT(".") + keyName;
	}
	
	list.Add(new FLuaTableRef(keyName, parent, hasNext));
}

void FLuaMemoryInfo::SaveRelationship(FString savePath)
{
	int nFileId = 0;
	FString tempFilePath = FPaths::GetPath(savePath);
	if(!IFileManager::Get().DirectoryExists(*tempFilePath))
	{
		IFileManager::Get().MakeDirectory(*tempFilePath, true);
	}
	tempFilePath = FString::Printf(TEXT("%s_%s_funRelationship_%d.csv"),*savePath,*StrUIPanelName,  nFileId);
	while (FPaths::FileExists(tempFilePath))
	{
		nFileId++;
		tempFilePath = FString::Printf(TEXT("%s_%s_funRelationship_%d.csv"), *savePath,*StrUIPanelName,  nFileId);
	}

	FString strResult;
	
	for (auto &fun : FunToName)
	{
		if(StrUIPanelName.Len() > 1 && !fun.Value.Contains(StrUIPanelName))
			continue;
		TArray<FString> paths;
		const TArray<FLuaTableRef*>& infos = LuaObjRefInfo.FindChecked(fun.Key);
		TArray<void*> hadAddParent;
		GetParentPaths(infos,TEXT(""), paths, hadAddParent);
		strResult.Appendf(TEXT("\r\n%p,%s"), fun.Key, *fun.Value);
		for(int i = 0; i < paths.Num(); i++)
		{
			strResult.Appendf(TEXT(",%s"),*paths[i]);
		}
	}
	FFileHelper::SaveStringToFile(strResult, *tempFilePath);
#if WITH_EDITOR
	strResult.Empty();
	for(auto& item :  IUnLuaModule::Get().GetEnv()->GetDelegateRegistry()->GetAllHandler())
	{
		strResult.Appendf(TEXT("%s,%d\r\n"), *item.Key.luaFunPath, item.Value->GetRefCount());
	}
	tempFilePath += TEXT(".delegate.csv");
	FFileHelper::SaveStringToFile(strResult, *tempFilePath);
#endif	
/*
	strResult.Empty();
	for(auto& item : LuaObjRefInfo)
	{
		strResult.Appendf(TEXT("%p,%d\r\n"), item.Key, item.Value.Num());
	}
	tempFilePath += TEXT(".all.csv");
	FFileHelper::SaveStringToFile(strResult, *tempFilePath);
	*/
}

void FLuaMemoryInfo::GetParentPaths(const TArray<FLuaTableRef*>& parents,FString key,  TArray<FString>& getpath, TArray<void*>&hadAddParent)
{
	for(int i = 0; i < parents.Num(); i++)
	{
		FLuaTableRef* pRef = parents[i];
		if(pRef->hasNext)
		{
			if(hadAddParent.Contains(pRef->pParent))
				continue;
			hadAddParent.Add(pRef->pParent);
			TArray<FString>Temppaths;
			if(!LuaObjRefInfo.Contains(pRef->pParent))
				continue;
			const TArray<FLuaTableRef*>& infos = LuaObjRefInfo.FindChecked(pRef->pParent);
			GetParentPaths(infos, pRef->key, Temppaths, hadAddParent);
			for(int pathIndex = 0; pathIndex < Temppaths.Num();pathIndex++)
			{
				FString temp = key + TEXT(" -> ") + Temppaths[pathIndex];
				getpath.Add(temp);
			}
		}
		else
		{
			FString temp = key + TEXT(" -> ") + pRef->key;
			getpath.Add(temp);
			//getpath.Add(FString::Printf(TEXT("%s -> %s"), *key, *pRef->key));
		}
	}
}


int32 FLuaMemoryInfo_PrintMem(lua_State *L)
{
	if(lua_gettop(L) ==0 ||  lua_type(L,1) != LUA_TSTRING)
	{
		UE_LOG(LogTemp,Display,TEXT("[Lua]FLuaMemoryInfo.PrintMem param error"))
		return 0;
	}
	FLuaMemoryInfo MemoryInfo;
	MemoryInfo.AnalyzeGcObj();
	FString savePath(UTF8_TO_TCHAR(lua_tostring(L,1)));	
	MemoryInfo.SaveResult(savePath);	
	return 0;
}


int32 FLuaMemoryInfo_LuaObjRelation(lua_State *L)
{
	if(lua_gettop(L) ==0 ||  lua_type(L,1) != LUA_TSTRING)
	{
		UE_LOG(LogTemp,Display,TEXT("[Lua]FLuaMemoryInfo.PrintMem param error"))
		return 0;
	}
	FLuaMemoryInfo MemoryInfo;
	if(lua_gettop(L) > 1 && lua_type(L,2) == LUA_TSTRING)
	{
		MemoryInfo.GetObjRelation(UTF8_TO_TCHAR(lua_tostring(L,2)));
	}
	else
		MemoryInfo.GetObjRelation();
	FString savePath(UTF8_TO_TCHAR(lua_tostring(L,1)));	
	MemoryInfo.SaveRelationship(savePath);
	return 0;
}

static const luaL_Reg FLuaMemoryInfoLib[] =
{
	{"PrintMem", FLuaMemoryInfo_PrintMem},
	{"LuaFuns",FLuaMemoryInfo_LuaObjRelation},
	{ nullptr, nullptr }
};

EXPORT_UNTYPED_CLASS(FLuaMemoryInfo, false, FLuaMemoryInfoLib)
IMPLEMENT_EXPORTED_CLASS(FLuaMemoryInfo)
