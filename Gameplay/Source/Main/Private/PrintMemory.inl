#pragma once
#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"


int ULuaManger_LuaFullGc(lua_State *L)
{
    static lu_mem nNextGcCount = 0;    
    lu_mem nCount = gettotalbytes(G(L));
    lu_mem nShouldCheckCount = 209715200; //200M
    int nParamNum = lua_gettop(L);
    if(nParamNum > 0 && lua_type(L,1) == LUA_TNUMBER)
        nShouldCheckCount = lua_tointeger(L, 1);
    if(nParamNum > 1 && lua_type(L,2) == LUA_TNUMBER)
    {
        lu_mem nForceFullGc = lua_tointeger(L,2);
        if(nCount > nForceFullGc)
        {
            lua_gc(L, LUA_GCCOLLECT);
            nNextGcCount = gettotalbytes(G(L)) * 2;
            UE_LOG(GAME,Display,TEXT("[main]lua force full gc,next gc is %lld"), nNextGcCount);
            return 0;
        }
    }
    if(nCount > nShouldCheckCount && nCount > nNextGcCount)
    {
        lua_gc(L, LUA_GCCOLLECT);
        nNextGcCount = gettotalbytes(G(L)) * 2;
        UE_LOG(GAME, Display, TEXT("[main]lua full gc,next gc is %lld"), nNextGcCount);
    }
    return 0;
}

int32 ULuaManger_LuaGcMem(lua_State *L)
{
    lu_mem majorbase = G(L)->GCestimate;  /* memory after last major collection */
    lu_mem majorinc = (majorbase / 100) * getgcparam(G(L)->genmajormul);
    UE_LOG(GAME,Display,TEXT("lua gc info,GCdebt=%lld,gettotalbytes=%lld, majorbase + majorinc=%lld, genmajormul=%d,genminormul=%d"),
        G(L)->GCdebt,gettotalbytes(G(L)),majorbase + majorinc,getgcparam(G(L)->genmajormul),G(L)->genminormul);
    return 0;
}

struct StatObjectInfo
{
    StatObjectInfo()
    {
        nTimes = 0;
        nTotalSize = 0;
        nSingleSize = 0;
    }
    int nTimes;
    int nTotalSize;
    int nSingleSize;
    TMap<FString, int> objNames;
};


UObject* GetTopWidgetForLog(UWidget* pCtrl)
{
    if(!IsValid(pCtrl))
        return nullptr;
    UObject* pTopParent = pCtrl;
    UWidget* pTemp = Cast<UWidget>(pCtrl->GetOuter());
    UWidgetTree* pTrees  = Cast<UWidgetTree>(pCtrl->GetOuter());
    
    while (pTemp != nullptr || pTrees != nullptr)
    {
        if(pTemp != nullptr)
        {
            pTopParent = pTemp;
            pTrees  = Cast<UWidgetTree>(pTemp->GetOuter());
            pTemp = Cast<UWidget>(pTemp->GetOuter());
        }else
        {
            pTemp = Cast<UWidget>(pTrees->GetOuter());            
            if(!pTemp && pTrees->GetOuter())
            {
                pTopParent = pTrees->GetOuter();
                break;
            }
            pTrees  = Cast<UWidgetTree>(pTrees->GetOuter());
        }
    }
    return pTopParent;
}


int ULuaManger_PrintObjectByClassName(lua_State *L)
{
    if(lua_gettop(L) < 2 || lua_type(L,1) != LUA_TSTRING || lua_type(L,2) != LUA_TSTRING )
    {
        UE_LOG(GAME,Warning,TEXT("[main]StatObjects param error"));
        return 0;
    }
    
    FString strsavePath(UTF8_TO_TCHAR(lua_tostring(L,1)));
    FString strClassName(UTF8_TO_TCHAR(lua_tostring(L,2)));
    FString otherInfo;
    for(int i = GUObjectArray.GetObjectArrayNum() - 1; i >= 0; i--)
    {
        auto item = GUObjectArray.IndexToObject(i);
        if(item->Object && item->Object->IsValidLowLevel())
        {            
            UClass* pClass = item->Object->GetClass();
            if(pClass->GetName().Contains(strClassName))
            {
                otherInfo.Appendf(TEXT("%s\r\n"),*item->Object->GetFName().ToString());          
            }
            
        }
    }
    FString strPath=  FPaths::GetPath(strsavePath);
    ULuaManger::Get()->CreateDir(strPath);
    int nFileId = 0;
    strPath = strsavePath;
    while (FPaths::FileExists(strPath))
    {
        nFileId++;
        strPath = FString::Printf(TEXT("%s_%d.csv"), *strsavePath, nFileId);
    }
    ULuaManger::Get()->SaveStringToFile(strPath, otherInfo);
    return 0;
}


int ULuaManger_PrintLiveObjects(lua_State *L)
{
    if(lua_gettop(L) < 1 || lua_type(L,1) != LUA_TSTRING)
    {
        UE_LOG(GAME,Warning,TEXT("[main]StatObjects param error"));
        return 0;
    }
    bool bLogObjectName= false;
    if(lua_gettop(L) > 1 && lua_type(L,2) == LUA_TBOOLEAN)
        bLogObjectName = lua_toboolean(L,2) != 0;
    FString strClassName;
    if(lua_gettop(L) > 2 && lua_type(L,3) == LUA_TSTRING)
        strClassName = ANSI_TO_TCHAR(lua_tostring(L,3));
    FString strsavePath(UTF8_TO_TCHAR(lua_tostring(L,1)));
    TMap<UClass*, StatObjectInfo> allObjects;
    int nTotalNum = 0;
    int nTotalSize = 0;
    FString otherInfo;
    for(int i = GUObjectArray.GetObjectArrayNum() - 1; i >= 0; i--)
    {
        auto item = GUObjectArray.IndexToObject(i);
        if(item->Object && item->Object->IsValidLowLevel())
        {            
            nTotalNum++;
            UClass* pClass = item->Object->GetClass();
            if(strClassName.Len() > 1 && pClass->GetName().Contains(strClassName))
            {
                if(UWidget* pTmp = Cast<UWidget>(item->Object->GetOuter()) )
                {
                    UObject* pMostOut = GetTopWidgetForLog(pTmp);
                    if(pMostOut)
                        otherInfo.Appendf(TEXT("%s,%s,%s,%p\r\n"),*item->Object->GetFName().ToString(), *item->Object->GetOuter()->GetName(), *pMostOut->GetName(),pMostOut);
                    else
                        otherInfo.Appendf(TEXT("%s,%s,%s\r\n"),*item->Object->GetFName().ToString(), *item->Object->GetOuter()->GetName(), *pClass->GetOutermost()->GetName());
                }
                else
                    otherInfo.Appendf(TEXT("%s,%s,%s,%p\r\n"),*item->Object->GetFName().ToString(), *item->Object->GetOuter()->GetName(), *pClass->GetOutermost()->GetName(), item->Object);                
            }
            StatObjectInfo& info = allObjects.FindOrAdd(pClass);
            int nSize = pClass->GetPropertiesSize();
            nTotalSize += nSize;
            info.nTotalSize += nSize;
            info.nTimes++;
            info.nSingleSize = FMath::Max(info.nSingleSize, nSize);
            if(bLogObjectName && !info.objNames.Contains(item->Object->GetFName().ToString()))
                info.objNames.Add(item->Object->GetFName().ToString());
        }
    }
    FString strInfo;
    strInfo.Appendf(TEXT("total,%d,1,%d\r\n"), nTotalNum, nTotalSize);
    for(auto& item:allObjects)
    {
        strInfo.Appendf(TEXT("%s,%d,%d,%d"), *item.Key->GetName(), item.Value.nTimes, item.Value.nSingleSize, item.Value.nTotalSize);
        if(bLogObjectName)
        {
            for(auto& name:item.Value.objNames)
            {
                strInfo.Appendf(TEXT(",%s"),*name.Key);
            }
        }
        strInfo.Append(TEXT("\r\n"));
    }
    FString strPath=  FPaths::GetPath(strsavePath);
    ULuaManger::Get()->CreateDir(strPath);
    int nFileId = 0;
    strPath = strsavePath;
    while (FPaths::FileExists(strPath))
    {
        nFileId++;
        strPath = FString::Printf(TEXT("%s_%d.%s"), *FPaths::GetBaseFilename(strsavePath,false), nFileId, *FPaths::GetExtension(strsavePath));
    }
    ULuaManger::Get()->SaveStringToFile(strPath, strInfo);
    if(strClassName.Len() > 1)
    {
        strPath = FString::Printf(TEXT("%s_%s_%d.csv"), *strsavePath,*strClassName, nFileId);
        ULuaManger::Get()->SaveStringToFile(strPath, otherInfo);
    }
    return 0;
}

#if ENABLE_MEMORYTRACKER
int ULuaManger_SetMemoryTracker(lua_State *L)
{
    if(lua_gettop(L) < 1 || lua_type(L,1) != LUA_TBOOLEAN)
    {
        UE_LOG(GAME, Error,TEXT("ULuaManger_SetMemoryTracker  param is not bool."))
        return 0;
    }
    int nIngore = 0;
    int nStack = 0;
    bool bTarcker = lua_toboolean(L,1) != 0;
    if(lua_gettop(L) > 1 && lua_type(L,2) == LUA_TNUMBER)
        nIngore = (int)lua_tointeger(L,2);
    if(lua_gettop(L) > 2 && lua_type(L,3) == LUA_TNUMBER)
        nStack = (int)lua_tointeger(L,3);
    FMemoryTracker::Get()->SetTracker(bTarcker, nIngore, nStack);
    
    return 0;
}

int ULuaManger_SaveMemoryTracker(lua_State *L)
{
    if(lua_gettop(L) < 2 || lua_type(L,1) != LUA_TSTRING)
    {
        UE_LOG(GAME, Error,TEXT("ULuaManger_SaveMemoryTracker  param less 2."))
        return 0;
    }
    FString savePath(UTF8_TO_TCHAR(lua_tostring(L,1)));
    if(lua_type(L,2) == LUA_TSTRING)
    {
        FString Tag(UTF8_TO_TCHAR(lua_tostring(L,2)));
        FMemoryTracker::Get()->SaveTag(Tag, savePath);
    }
    else if(lua_type(L,2) == LUA_TNUMBER)
    {
        FMemoryTracker::Get()->SaveTag((uint64)lua_tonumber(L,2), savePath);
    }
    else
    {
        UE_LOG(GAME, Error,TEXT("ULuaManger_SaveMemoryTracker  param 2 type error."))
    }
    
    return 0;
}

int ULuaManger_AddMemoryTrackerTag(lua_State *L)
{
    if(lua_gettop(L) < 1 )
    {
        UE_LOG(GAME, Error,TEXT("ULuaManger_AddMemoryTrackerTag  param error."))
        return 0;
    }
    if(lua_type(L,1) == LUA_TSTRING)
    {
        uint64 nTag = 0;
        
        FString Tag(UTF8_TO_TCHAR(lua_tostring(L,1)));
        FLowLevelMemTracker::Get().FindTagByName(*Tag, nTag);
        UE_LOG(LogTemp,Display,TEXT("llmtag %s --> %lld"),  *Tag, nTag);
        FMemoryTracker::Get()->AddTag(Tag);
    }
    else if(lua_type(L,1) == LUA_TNUMBER)
    {
        uint64 nTag = lua_tonumber(L,1);
        FString str = FLowLevelMemTracker::Get().FindTagDisplayName(nTag).ToString();
        UE_LOG(LogTemp,Display,TEXT("llmtag %lld-> %s"), nTag, *str);
        FMemoryTracker::Get()->AddTag(nTag);
    }
    else
    {
        UE_LOG(GAME, Error,TEXT("ULuaManger_AddMemoryTrackerTag  param 1 type error."))
    }
    
    return 0;
}

int ULuaManger_RemoveMemoryTrackerTag(lua_State *L)
{
    if(lua_gettop(L) < 1 )
    {
        UE_LOG(GAME, Error,TEXT("ULuaManger_RemoveMemoryTrackerTag  param error."))
        return 0;
    }
    if(lua_type(L,1) == LUA_TSTRING)
    {
        uint64 nTag = 0;
        
        FString Tag(UTF8_TO_TCHAR(lua_tostring(L,1)));
        FLowLevelMemTracker::Get().FindTagByName(*Tag, nTag);
        UE_LOG(LogTemp,Display,TEXT("llmtag %s --> %lld"),  *Tag, nTag);
        FMemoryTracker::Get()->RemoveTag(Tag);
    }
    else if(lua_type(L,1) == LUA_TNUMBER)
    {
        uint64 nTag = lua_tonumber(L,1);
        FString str = FLowLevelMemTracker::Get().FindTagDisplayName(nTag).ToString();
        UE_LOG(LogTemp,Display,TEXT("llmtag %lld-> %s"), nTag, *str);
        FMemoryTracker::Get()->RemoveTag(nTag);
    }
    else
    {
        UE_LOG(GAME, Error,TEXT("ULuaManger_RemoveMemoryTrackerTag  param 1 type error."))
    }
    
    return 0;
}

int ULuaManger_ClearMemoryTracker(lua_State *L)
{
    FMemoryTracker::Get()->Clear();
    
    return 0;
}

#endif