#include "FunctionRegistry.h"
#include "lua.hpp"
#include "LuaEnv.h"

namespace UnLua
{
    FFunctionRegistry::FFunctionRegistry(FLuaEnv* Env)
        : Env(Env)
    {
    }

    void FFunctionRegistry::NotifyUObjectDeleted(UObject* Object)
    {
        const auto Function = (ULuaFunction*)Object;
        const auto Info = LuaFunctions.Find(Function);
        if (!Info)
            return;
        luaL_unref(Env->GetMainState(), LUA_REGISTRYINDEX, Info->LuaRef);
        LuaFunctions.Remove(Function);
    }

    lua_Integer GetLuaFunctionRef(lua_State* L,int objRef, const char* funName)
    {
        lua_Integer FuncRef = LUA_NOREF;
        lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
        lua_getmetatable(L, -1);
        do
        {
            lua_pushstring(L, funName);
            lua_rawget(L, -2);
            if (lua_isfunction(L, -1))
            {
                lua_pushvalue(L, -3);
                lua_remove(L, -3);
                lua_remove(L, -3);
                lua_pushvalue(L, -2);
                FuncRef = luaL_ref(L, LUA_REGISTRYINDEX);
                break;
            }
            lua_pop(L, 1);
            lua_pushstring(L, "Super");
            lua_rawget(L, -2);
            lua_remove(L, -2);
        }
        while (lua_istable(L, -1));
        lua_pop(L, 2);
        return  FuncRef;
    }

    void FFunctionRegistry::Invoke(ULuaFunction* Function, UObject* Context, FFrame& Stack, RESULT_DECL)
    {
        // TODO: refactor
        if (UNLIKELY(!Env->GetObjectRegistry()->IsBound(Context)))
            Env->TryBind(Context);

        const auto SelfRef = Env->GetObjectRegistry()->GetBoundRef(Context);
        check(SelfRef!=LUA_NOREF);

        const auto L = Env->GetMainState();
        lua_Integer FuncRef;
        FFunctionDesc* FuncDesc;

        const auto Exists = LuaFunctions.Find(Function);
        if (Exists)
        {
            FuncRef = Exists->LuaRef;
            FuncDesc = Exists->Desc.Get();
        }
        else
        {
            FuncRef = LUA_NOREF;
            FuncDesc = new FFunctionDesc(Function, nullptr, false);
            //如果是 FUNC_NET 先查找_RPC,未找到再查找一般名字。如果不是FUNC_NET，则只查找一般名字
            FuncRef = GetLuaFunctionRef(L,SelfRef, FuncDesc->GetLuaFunctionName());            
            if(FuncRef == LUA_NOREF)
            {
                if (Function->HasAnyFunctionFlags(FUNC_Net))
                    FuncRef = GetLuaFunctionRef(L,SelfRef, TCHAR_TO_UTF8(*Function->GetName()));
            }            
            
            FFunctionInfo Info;
            Info.LuaRef = FuncRef;
            Info.Desc = TUniquePtr<FFunctionDesc>(FuncDesc);
            LuaFunctions.Add(Function, MoveTemp(Info));
        }

        if (FuncRef == LUA_NOREF)
        {
            // 可能因为Lua模块加载失败导致找不到对应的function，转发给原函数
            const auto Overridden = Function->GetOverridden();
            if (Overridden && Stack.Code)
                Overridden->Invoke(Context, Stack, RESULT_PARAM);
            return;
        }
        FuncDesc->CallLua(L, FuncRef, SelfRef, Stack, RESULT_PARAM);
    }
}
