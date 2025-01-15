#pragma once
#include "rapidjson/document.h"

#include "LuaMessageTableOptInC.h"



class FJsonToLuaTable
{
public:
	static const Table*  ConvertJsonToLuaTable( const char* strJson, lua_State* L){
		if(strJson == nullptr)
    		return nullptr;
    	
		rapidjson::Document doc;
		doc.Parse(strJson);
		if(doc.HasParseError())
		{
			UE_LOG(LogTemp,Warning,TEXT("parse json error,please check!"));
			return nullptr;
		}
		if(doc.IsArray())
		{
			return ParseArray(doc,L, true);
		}
		if(doc.IsObject())
		{
			return ParseObject(doc,L, true);
		}
		return nullptr;
	}

private:
	static Table*  ParseArray(rapidjson::Value& js_value, lua_State* L, bool bPushToTop = false)
	{
		Table* pTable = CLuaMessageTableOptInC::CreateNewTable(nullptr, js_value.Size(), 1, L, bPushToTop);
		for (unsigned int i = 0; i < js_value.Size(); i++)
		{
			ParseJson(js_value[i], pTable, i+1,L);
		}
		return pTable;
	}
	static Table*  ParseObject(rapidjson::Value& js_value, lua_State* L, bool bPushToTop = false)
	{
		Table* pTable = CLuaMessageTableOptInC::CreateNewTable(nullptr,1,1,L,bPushToTop);
		for(auto iter = js_value.MemberBegin(); iter != js_value.MemberEnd(); ++iter)
		{
			auto key = (iter->name).GetString();
			ParseJson(js_value[key], pTable, key,L);
		}
		return pTable;
	}
	/*
	static Table*  ParseJson(rapidjson::Value& js_value, lua_State* L)
	{
		if(js_value.IsArray())
		{
			return ParseArray(js_value,L);
		}
		if(js_value.IsObject())
		{
			return ParseObject(js_value,L);
		}
		
		UE_LOG(LogTemp,Display,TEXT("json is error"))
		return nullptr;
		
	}*/

	static void ParseJson(rapidjson::Value& js_value, Table* pTable, int nIndex, lua_State* L)
	{
		if(js_value.IsArray())
		{
			Table* subTable = ParseArray(js_value,L);
			CLuaMessageTableOptInC::AddTable(pTable, nIndex, subTable, L);
		}
		else if(js_value.IsObject())
		{
			Table* subTable = ParseObject(js_value,L);
			CLuaMessageTableOptInC::AddTable(pTable, nIndex, subTable, L);
		}
		else
		{
			TValue luaValue;
			ParseBaseType(js_value, &luaValue,L);
			CLuaMessageTableOptInC::AddTValue(pTable, nIndex, &luaValue,L);
		}
	}

	static void ParseJson(rapidjson::Value& js_value, Table* pTable, const char* key, lua_State* L)
	{
		if(js_value.IsArray())
		{
			Table* subTable = ParseArray(js_value,L);
			CLuaMessageTableOptInC::AddTable(pTable, key, subTable, L);
		}
		else if(js_value.IsObject())
		{
			Table* subTable = ParseObject(js_value,L);
			CLuaMessageTableOptInC::AddTable(pTable, key, subTable, L);
		}
		else
		{
			TValue luaKey;
			TString* toLuaStr =  luaS_new(L, key);
			setsvalue(L, &luaKey, toLuaStr);
			TValue luaValue;
            ParseBaseType(js_value, &luaValue,L);
			CLuaMessageTableOptInC::AddValue(pTable, &luaKey, &luaValue,L);
		}
	}

	static void ParseBaseType(rapidjson::Value& js_value, TValue* pValue, lua_State* L)
	{
		if(pValue == nullptr)
			return;
		if(js_value.IsBool())
		{
			CLuaMessageTableOptInC::SetBoolValue(pValue, js_value.GetBool());
		}
		else if(js_value.IsString())
		{
			TString* toLuaStr =  luaS_new(L, js_value.GetString());
			setsvalue(L, pValue, toLuaStr);
		}
		else if(js_value.IsInt())
		{
			setivalue(pValue, js_value.GetInt());
		}
		else if(js_value.IsUint())
		{
			setivalue(pValue, js_value.GetUint());
		}
		else if(js_value.IsInt64())
		{
			setivalue(pValue, js_value.GetInt64());
		}
		else if(js_value.IsUint64())
		{
			setivalue(pValue, js_value.GetUint64());
		}
		else if(js_value.IsDouble())
		{
			setfltvalue(pValue, js_value.GetDouble());
		}
		else
		{
			setnilvalue(pValue);
		}
	}
};