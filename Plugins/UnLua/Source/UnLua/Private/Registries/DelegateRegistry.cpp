﻿// Tencent is pleased to support the open source community by making UnLua available.
// 
// Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the MIT License (the "License"); 
// you may not use this file except in compliance with the License. You may obtain a copy of the License at
//
// http://opensource.org/licenses/MIT
//
// Unless required by applicable law or agreed to in writing, 
// software distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and limitations under the License.

#include "Registries/DelegateRegistry.h"
#include "LuaDelegateHandler.h"
#include "ObjectReferencer.h"
#include "LuaEnv.h"
#include "LuaMessageTableOptInC.h"

namespace UnLua
{
    FDelegateRegistry::FDelegateRegistry(FLuaEnv* Env)
        : Env(Env)
    {
        PostGarbageCollectHandle = FCoreUObjectDelegates::GetPostGarbageCollect().AddRaw(this, &FDelegateRegistry::OnPostGarbageCollect);
    }

    FDelegateRegistry::~FDelegateRegistry()
    {
        for (auto& Pair : CachedHandlers)
        {
            const auto ToRelease = Pair.Value.Get();
            if (!ToRelease)
                continue;
            ToRelease->Reset();
            Env->AutoObjectReference.Remove(ToRelease);
        }
        Delegates.Empty();
        FCoreUObjectDelegates::GetPostGarbageCollect().Remove(PostGarbageCollectHandle);
    }

    void FDelegateRegistry::OnPostGarbageCollect()
    {
        TArray<TTuple<void*, FDelegateInfo>> InvalidPairs;
        for (auto& Pair : Delegates)
        {
            if (!Pair.Value.Owner.IsValid())
                InvalidPairs.Add(Pair);
        }

        for (int i = 0; i < InvalidPairs.Num(); i++)
        {
            const auto& Pair = InvalidPairs[i];
            if (Pair.Value.bIsMulticast)
                Clear(Pair.Key);
            else
                Unbind(Pair.Key);
            Delegates.Remove(Pair.Key);
        }

        TArray<FLuaDelegatePair> ToRemove;
        for (auto& Pair : CachedHandlers)
        {
            if (Pair.Key.SelfObject.IsStale())
            {
                ToRemove.Add(Pair.Key);
                Env->AutoObjectReference.Remove(Pair.Value.Get());
            }
            else if(Pair.Value->RefCount <= 0)
            {
                for(auto& item:Delegates)
                {
                    if(item.Value.Handlers.Contains(Pair.Value))
                        item.Value.Handlers.Remove(Pair.Value);
                }
                ToRemove.Add(Pair.Key);
                Env->AutoObjectReference.Remove(Pair.Value.Get());
            }
        }

        for (auto& Key : ToRemove)
        {
            TWeakObjectPtr<ULuaDelegateHandler> Handler;
            if (CachedHandlers.RemoveAndCopyValue(Key, Handler))
            {
                const auto L = Env->GetMainState();
                luaL_unref(L, LUA_REGISTRYINDEX, Handler->LuaRef);
                Handler->Reset();
            }
        }
    }

    void FDelegateRegistry::NotifyHandlerBeginDestroy(ULuaDelegateHandler* Handler)
    {
        const auto L = Env->GetMainState();
        luaL_unref(L, LUA_REGISTRYINDEX, Handler->LuaRef);
        Handler->Reset();
    }

#pragma region FScriptDelgate

    void FDelegateRegistry::Register(void* Delegate, FProperty* Property, UObject* Owner)
    {
        check(!Owner || Owner->IsValidLowLevelFast());
        const auto Info = Delegates.Find(Delegate);
        if (Info)
        {
            check(Info->Property == Property);
            Info->Owner = Owner;
        }
        else
        {
            FDelegateInfo NewInfo;
            NewInfo.Property = Property;
            NewInfo.Desc = nullptr;
            if (const auto MulticastProperty = CastField<FMulticastDelegateProperty>(Property))
            {
                NewInfo.SignatureFunction = MulticastProperty->SignatureFunction;
                NewInfo.bIsMulticast = true;
            }
            else if (const auto DelegateProperty = CastField<FDelegateProperty>(Property))
            {
                NewInfo.SignatureFunction = DelegateProperty->SignatureFunction;
                NewInfo.bIsMulticast = false;
            }
            else
            {
                check(false);
            }
            NewInfo.Owner = Owner;
            Delegates.Add(Delegate, NewInfo);
        }
    }

    void FDelegateRegistry::Bind(lua_State* L, int32 Index, FScriptDelegate* Delegate, UObject* SelfObject)
    {
        check(lua_type(L, Index) == LUA_TFUNCTION);
        const auto LuaFunction = lua_topointer(L, Index);
        auto& Info = Delegates.FindChecked(Delegate);
        if (!Info.Owner.IsValid())
            Info.Owner = SelfObject;

        auto DelegatePair = FLuaDelegatePair(SelfObject, LuaFunction);
        const auto Cached = CachedHandlers.Find(DelegatePair);
        if (Cached && Cached->IsValid())
        {
            (*Cached)->BindTo(Delegate);
            Info.Handlers.Add(*Cached);
            (*Cached)->RefCount++;
            return;
        }
#if WITH_EDITOR
        lua_Debug ar;
        int line = 0;
        const char* name = "";
        if (lua_getstack(L, 1, &ar)) {
            lua_getinfo(L, "nSl", &ar);
            line = ar.currentline;
            name = ar.source;
        } 
        DelegatePair.luaFunPath = FString::Printf(TEXT("LuaFunc_[%s:%d]_CppDelegate"), ANSI_TO_TCHAR(name), line);
#endif

        lua_pushvalue(L, Index);
        const auto Ref = luaL_ref(L, LUA_REGISTRYINDEX);
        const auto Handler = CreateHandler(Ref, Info.Owner.Get(), SelfObject);
        Handler->BindTo(Delegate);
        Env->AutoObjectReference.Add(Handler);
        CachedHandlers.Add(DelegatePair, Handler);
        Info.Handlers.Add(Handler);
    }

    void FDelegateRegistry::Unbind(void* Delegate)
    {
        const auto Info = Delegates.Find(Delegate);
        if (!Info)
            return;

        for (const auto& Handler : Info->Handlers)
        {
            if (!Handler.IsValid())
                continue;
            if (!Info->Owner.IsStale())
            {
                if(((FScriptDelegate*)Delegate)->IsBound())
                    Handler->RefCount--;
                ((FScriptDelegate*)Delegate)->Unbind();
            }
        }
        Info->Handlers.Empty();
    }

    void FDelegateRegistry::Execute(const ULuaDelegateHandler* Handler, void* Params)
    {
        const auto SignatureDesc = GetSignatureDesc(Handler->Delegate);
        if (!SignatureDesc)
            return;

        if (Handler->SelfObject.IsStale())
            return;
        if(Handler->RefCount <= 0)
        {
            UE_LOG(LogUnLua,Warning,TEXT("FDelegateRegistry::Execute refcount=%d"), Handler->RefCount)
        }

        const auto L = Env->GetMainState();
        SignatureDesc->CallLua(L, Handler->LuaRef, Params, Handler->SelfObject.Get());
    }

    int32 FDelegateRegistry::Execute(lua_State* L, FScriptDelegate* Delegate, int32 NumParams, int32 FirstParamIndex)
    {
        const auto SignatureDesc = GetSignatureDesc(Delegate);
        if (!SignatureDesc)
            return 0;

        const auto Ret = SignatureDesc->ExecuteDelegate(L, NumParams, FirstParamIndex, Delegate);
        return Ret;
    }

#pragma endregion

#pragma region FMulticastScriptDelegate

    void FDelegateRegistry::Add(lua_State* L, int32 Index, void* Delegate, UObject* SelfObject)
    {
        check(lua_type(L, Index) == LUA_TFUNCTION);
        auto& Info = Delegates.FindChecked(Delegate);
        if (!Info.Owner.IsValid())
            Info.Owner = SelfObject;

        const auto LuaFunction = lua_topointer(L, Index);
        auto DelegatePair = FLuaDelegatePair(SelfObject, LuaFunction);
        const auto Cached = CachedHandlers.Find(DelegatePair);
        if (Cached && Cached->IsValid())
        {
            (*Cached)->AddTo(Info.MulticastProperty, Delegate);
            Info.Handlers.Add(*Cached);
            (*Cached)->RefCount++;
            return;
        }
#if WITH_EDITOR
        lua_Debug ar;
        int line = 0;
        const char* name = "";
        if (lua_getstack(L, 1, &ar)) {
            lua_getinfo(L, "nSl", &ar);
            line = ar.currentline;
            name = ar.source;
        }
        DelegatePair.luaFunPath = FString::Printf(TEXT("LuaFunc_[%s:%d]_CppDelegate"), ANSI_TO_TCHAR(name), line);
#endif
        
        lua_pushvalue(L, Index);
        const auto Ref = luaL_ref(L, LUA_REGISTRYINDEX);
        const auto Handler = CreateHandler(Ref, Info.Owner.Get(), SelfObject);
        Env->AutoObjectReference.Add(Handler);
        Handler->AddTo(Info.MulticastProperty, Delegate);
        CachedHandlers.Add(DelegatePair, Handler);
        Info.Handlers.Add(Handler);
    }

    void FDelegateRegistry::Remove(lua_State* L, UObject* SelfObject, void* Delegate, int Index)
    {
        check(lua_type(L, Index) == LUA_TFUNCTION);
        const auto LuaFunction = lua_topointer(L, Index);
        auto& Info = Delegates.FindChecked(Delegate);

        const auto DelegatePair = FLuaDelegatePair(SelfObject, LuaFunction);
        const auto Cached = CachedHandlers.Find(DelegatePair);
        if (!Cached || !Cached->IsValid())
            return;
        if((*Cached)->Contain(Info.MulticastProperty, Delegate))            
            (*Cached)->RefCount--;
        else
        {
            //UE_LOG(LogUnLua,Warning,TEXT("FDelegateRegistry remove not add %d"),(*Cached)->RefCount);
            //CLuaMessageTableOptInC::OnlyPrintLuaTrackback("",L);
        }

        (*Cached)->RemoveFrom(Info.MulticastProperty, Delegate);
        Info.Handlers.Remove(*Cached);      
    }

    void FDelegateRegistry::Broadcast(lua_State* L, void* Delegate, int32 NumParams, int32 FirstParamIndex)
    {
        const auto SignatureDesc = GetSignatureDesc(Delegate);
        check(SignatureDesc);

        const auto Info = Delegates.Find(Delegate);
        const auto Property = Info->MulticastProperty;
        const auto ScriptDelegate = TMulticastDelegateTraits<FMulticastDelegateType>::GetMulticastDelegate(Property, Delegate);
        SignatureDesc->BroadcastMulticastDelegate(L, NumParams, FirstParamIndex, ScriptDelegate);
    }

    void FDelegateRegistry::Clear(void* Delegate)
    {
        const auto Info = Delegates.Find(Delegate);
        if (!Info)
            return;

        for (const auto& Handler : Info->Handlers)
        {
            if (!Handler.IsValid())
                continue;
            Handler->RefCount--;
            if (Info->Owner.IsValid())
            {                
                Handler->RemoveFrom(Info->MulticastProperty, Delegate);
            }
        }

        Info->Handlers.Empty();
    }

#pragma endregion

    TSharedPtr<FFunctionDesc> FDelegateRegistry::GetSignatureDesc(const void* Delegate)
    {
        const auto Info = Delegates.Find(Delegate);
        if (!Info)
            return nullptr;
        if (!Info->Desc)
            Info->Desc = MakeShared<FFunctionDesc>(Info->SignatureFunction, nullptr);
        return Info->Desc;
    }

    ULuaDelegateHandler* FDelegateRegistry::CreateHandler(int LuaRef, UObject* Owner, UObject* SelfObject)
    {
        const auto Ret = NewObject<ULuaDelegateHandler>();
        Ret->Registry = this;
        Ret->LuaRef = LuaRef;
        Ret->SelfObject = SelfObject ? SelfObject : Owner;
        Ret->RefCount = 1;
        return Ret;
    }
    void FDelegateRegistry::ReleaseHandle(const UObject* Object)
    {
        if(!Object)
            return;
        TWeakObjectPtr<ULuaDelegateHandler> deletgateHandel;
        FLuaDelegatePair* key = nullptr;
        for(auto& item:CachedHandlers)
        {
            if(item.Value == Object)
            {
                item.Value.Get()->RefCount--;
                deletgateHandel = item.Value;
                key = &item.Key;
                break;
            }
        }/*
        if(deletgateHandel.IsValid() && deletgateHandel->RefCount == 0)
        {
            for(auto& item:Delegates)
            {
                if(item.Value.Handlers.Contains(deletgateHandel))
                    item.Value.Handlers.Remove(deletgateHandel);
            }
            Env->AutoObjectReference.Remove(deletgateHandel.Get());
            CachedHandlers.Remove(*key);
        }*/
    }

    void FDelegateRegistry::ReleaseHandle(const UObject* Object, const void* luaFun)
    {
        if(!Object || !luaFun)
            return;
        const auto DelegatePair = FLuaDelegatePair((UObject*)Object, luaFun);
        const auto Cached = CachedHandlers.Find(DelegatePair);
        if(Cached && Cached->IsValid())
        {
            (*Cached)->RefCount--;/*
            if((*Cached)->RefCount == 0)
            {
                for(auto& item:Delegates)
                {
                    if(item.Value.Handlers.Contains(*Cached))
                        item.Value.Handlers.Remove(*Cached);
                }
                Env->AutoObjectReference.Remove(Cached->Get());
                CachedHandlers.Remove(DelegatePair);
            }*/
        }
    }
}
