#pragma once
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "LuaMessageTableOptInC.h"
#include <string>


class FLuaTableToJson
{
public:
	static bool ConvertLuaTableToJson(const Table* luaTable, FString& strJson){
		if(luaTable == nullptr)
    		return false;
    	rapidjson::StringBuffer buffer;
    	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    	
    	if(!ConvertLuaTableToJson(luaTable, &writer))
    		return false;
    	strJson = FString(UTF8_TO_TCHAR(buffer.GetString()));
    
    	return true;
	}

	static bool ConvertLuaTableToJson(const Table* luaTable, std::string & strJson){
		if(luaTable == nullptr)
			return false;
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    	
		if(!ConvertLuaTableToJson(luaTable, &writer))
			return false;
		strJson = buffer.GetString();
    
		return true;
	}

private:
	static bool ConvertLuaTableToJson(const Table* luaTable, rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter)
	{
		if(!luaTable || !jsonWriter)
			return false;
		bool bIsArray = false;
		if(!CheckTable(luaTable, bIsArray))
		{
			UE_LOG(LogCore, Error,TEXT("[json]luatable,has array and hash,check to only use one type"));
			return false;
		}
		unsigned int nCount = 0;
		if(bIsArray)
		{
			jsonWriter->StartArray();
			nCount = luaTable->alimit;
			for(unsigned int i = 1; i <= nCount; i++)
			{
				TValue* pTempValue = &luaTable->array[i-1];
				if(ttisnil(pTempValue))
					continue;
				if(!ConvertTValueToJson(pTempValue, jsonWriter))
					return false;
			}
			jsonWriter->EndArray();
		}
		else
		{
			jsonWriter->StartObject();
			nCount = 1 << luaTable->lsizenode;
			Node* pNode;
			for(unsigned int i = 0; i < nCount; i++)
			{
				pNode = &luaTable->node[i];
				if(pNode == nullptr)
					continue;			
				if(ttisnil(&pNode->i_val) || ttisfunction(&pNode->i_val))
				{
					continue;
				}
				TValue pTempValue;
				CLuaMessageTableOptInC::GetKeyFromNode(pNode, pTempValue);
				if(!ttisboolean(&pTempValue) && !ttisnumber(&pTempValue) && !ttisstring(&pTempValue))
					continue;
			
				std::string key( TValueToString(&pTempValue));
				jsonWriter->Key(key.c_str(),key.size());
		
				if(!ConvertTValueToJson(&pNode->i_val, jsonWriter))
					return false;
			}
			jsonWriter->EndObject();
		}
		return true;
	}
	static bool ConvertTValueToJson(const TValue* luaValue, rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter)
	{
		if(!luaValue || !jsonWriter)
			return false;
		if(ttisboolean(luaValue))
		{
			jsonWriter->Bool(CLuaMessageTableOptInC::GetBoolValue(luaValue));
		}
		else if(ttisinteger(luaValue))
		{
			jsonWriter->Int64(ivalue(luaValue));
		}
		else if(ttisfloat(luaValue))
		{
			double fTemp = fltvalue(luaValue);
			jsonWriter->Double(fTemp);
		}
		else if(ttisstring(luaValue))
		{
			std::string strtemp(svalue(luaValue));
			jsonWriter->String(strtemp.c_str(), strtemp.length(), true);
		}
		else if(ttistable(luaValue))
		{
			return ConvertLuaTableToJson(hvalue(luaValue), jsonWriter);
		}
		else
		{
			return false;
		}
		return true;
	}
	static std::string TValueToString(const TValue* luaValue)
	{
		std::string strKey;
		if (ttisboolean(luaValue))
		{
			strKey = CLuaMessageTableOptInC::GetBoolValue(luaValue) ? "true" : "false";
		}
		else if (ttisinteger(luaValue))
		{
			strKey = stringFormat("%lld", ivalue(luaValue));
		}
		else if (ttisfloat(luaValue))
		{
			strKey = stringFormat("%f", fvalue(luaValue));
		}
		else if (ttisstring(luaValue))
		{
			strKey = svalue(luaValue);
		}

		return strKey;
	}
	static bool CheckTable(const Table* luaTable, bool &isArray)
	{
		if(luaTable == nullptr)
			return false;
		isArray = false;
		unsigned int nCount = luaTable->alimit;
		for(unsigned int i = 1; i <= nCount; i++)
		{
			TValue* pValue = &luaTable->array[i-1];
			if(!ttisnil(pValue))
			{
				isArray = true;
				break;
			}
		}
		nCount = 1 << luaTable->lsizenode;
		Node* pNode;
		for(unsigned int i = 0; i < nCount; i++)
		{
			pNode = &luaTable->node[i];
			if(pNode == nullptr)
				continue;
			if(pNode->u.key_tt != LUA_TBOOLEAN &&
				pNode->u.key_tt != LUA_TNUMBER &&
				pNode->u.key_tt != LUA_TSTRING)
					continue;
			if(ttisnil(&pNode->i_val) || ttisfunction(&pNode->i_val))
			{
				continue;
			}
			if(isArray)
			{
				return false;
			}
		}
		return true;
	}

#define STRINGFORMAT_BUFFER_SIZE 512
	template<typename ... Args>
	static  std::string stringFormat(const char* format, Args ... args)
	{
		if(format == nullptr)
			return std::string();	
		char buffer[STRINGFORMAT_BUFFER_SIZE] = {0};
		snprintf(buffer, STRINGFORMAT_BUFFER_SIZE - 1, format, args ...);
		return std::string(buffer);
	}
};