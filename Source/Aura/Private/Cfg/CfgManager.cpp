
#include "CfgManager.h"

#include "CfgConstTable.h"
#include "lua.hpp"
#include "CfgTable.h"
#include "EndianOpt.h"
#include "LuaManger.h"
#include "LuaMessageTableOptInC.h"
#include "UnLua.h"
#include "Misc/FileHelper.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "UnLuaBase.h"
#include "rapidjson/document.h"

FCfgManager* FCfgManager::m_Ins = nullptr;

FCfgManager::FCfgManager()
{
	m_nBeanId = CFG_TYPE_BEAN + 1;
	m_nTableId = CFG_TYPE_TABLE;
	Init();
	FEndianOpt::Init();
}

FCfgManager* FCfgManager::Get()
{
	if(m_Ins == nullptr)
	{
		m_Ins = new FCfgManager();
	}
	return m_Ins;
}

void FCfgManager::Free()
{
	if(m_Ins)
	{
		m_Ins->UnInit();
		delete m_Ins;
		m_Ins = nullptr;
	}
}

void FCfgManager::Init()
{
	
}

void FCfgManager::UnInit()
{
	for(auto&item:m_idToTable)
	{
		delete item.Value;
	}
	m_idToTable.Empty();
	m_luatableToCTable.Empty();
	for(auto& item:FCfgTable::m_idToBeans)
	{
		delete item.Value;
	}
	FCfgTable::m_idToBeans.Empty();
	FCfgTable::m_NameToBeanId.Empty();
}

int FCfgManager::GetCfgType(const char* str)
{
	if(str == nullptr)
	{
		UE_LOG(LogTemp, Error,TEXT("[cfg]FCfgManager::GetCfgType str null"))
		return 0;
	}
	if(strcmp(str,"bool") == 0)
		return CFG_TYPE_BOOL;
	if(strcmp(str,"int") == 0)
		return CFG_TYPE_INT;
	if(strcmp(str,"double") == 0)
		return CFG_TYPE_DOUBLE;
	if(strcmp(str,"float") == 0)
		return CFG_TYPE_FLOAT;
	if(strcmp(str,"string") == 0)
		return CFG_TYPE_STRING;
	if(strcmp(str,"list") == 0)
		return CFG_TYPE_LIST;
	if(strcmp(str,"set") == 0)
		return CFG_TYPE_SET;
	if(strcmp(str,"map") == 0)
		return CFG_TYPE_MAP;
	FString strType(str);
	int* pId = FCfgTable::m_NameToBeanId.Find(strType);
	if(pId)
		return *pId;
	UE_LOG(LogTemp, Error,TEXT("[cfg]FCfgManager::GetCfgType error str=%s "),*strType);
	return 0;
}

bool FCfgManager::RegisBean(FString& info)
{
	rapidjson::Document doc;
	doc.Parse(TCHAR_TO_UTF8(*info));
	if(doc.IsArray())
	{
		UE_LOG(LogTemp, Error, TEXT("[cfg]FCfgManager::RegisBean error"));
		return false;
	}

	SCfgStructInfo* pStructInfo = new SCfgStructInfo();
	pStructInfo->m_cfgName = FString(doc["name"].GetString());
	pStructInfo->m_nId = m_nBeanId++;
	const rapidjson::Value& fields = doc["fields"];
	for (rapidjson::SizeType j = 0; j < fields.Size(); j++)
	{
		const rapidjson::Value& singleField = fields[j];
		SCfgStructItemType itemInfo;
		FString ItemName = singleField["name"].GetString();
		itemInfo.nType = GetCfgType(singleField["type"].GetString());
		if (singleField.HasMember("value"))
			itemInfo.nValue = GetCfgType(singleField["value"].GetString());
		if (singleField.HasMember("key"))
			itemInfo.nKeyType = GetCfgType(singleField["key"].GetString());
		pStructInfo->m_nameToIndex.Add(ItemName, pStructInfo->m_Items.Num());
		pStructInfo->m_Items.Add(itemInfo);
	}
	FCfgTable::m_idToBeans.Add(pStructInfo->m_nId, pStructInfo);
	FCfgTable::m_NameToBeanId.Add(pStructInfo->m_cfgName, pStructInfo->m_nId);
	FCfgTable::InitCfgStruct(pStructInfo);

	return true;
}


SCfgItemRow* FCfgManager::RegisConstTable(FString& info)
{
	rapidjson::Document doc;
	doc.Parse(TCHAR_TO_UTF8(*info));
	if (doc.IsArray())
	{
		UE_LOG(LogTemp, Error, TEXT("[cfg]FCfgManager::RegisConstTable error"));
		return nullptr;
	}
	SCfgStructInfo* pStructInfo = new SCfgStructInfo();
	pStructInfo->m_cfgName = FString(doc["name"].GetString());
	pStructInfo->m_nId = m_nTableId++;
	const rapidjson::Value& fields = doc["fields"];
	for (rapidjson::SizeType j = 0; j < fields.Size(); j++)
	{
		const rapidjson::Value& singleField = fields[j];
		SCfgStructItemType itemInfo;
		FString ItemName = singleField["name"].GetString();
		itemInfo.nType = GetCfgType(singleField["type"].GetString());
		if (singleField.HasMember("value"))
			itemInfo.nValue = GetCfgType(singleField["value"].GetString());
		if (singleField.HasMember("key"))
			itemInfo.nKeyType = GetCfgType(singleField["key"].GetString());
		pStructInfo->m_nameToIndex.Add(ItemName, pStructInfo->m_Items.Num());
		pStructInfo->m_Items.Add(itemInfo);
	}
	
	FCfgTable* pCfgTable = new FCfgConstTable(pStructInfo);
	m_idToTable.Add(pStructInfo->m_nId, pCfgTable);
	pCfgTable->InitTable();
	TArray<uint8> ocFile;
	FString strPath = GetOcFullPath(*pStructInfo->m_cfgName);
	FFileHelper::LoadFileToArray(ocFile, *strPath);
	pCfgTable->GenerateRows(ocFile.GetData(), ocFile.Num());
	uint8* pRow = pCfgTable->GetRow(0);
	SCfgItemRow* pItemRom = (SCfgItemRow*)pRow;
	if(FCfgTable::PushItemRowToLua(pItemRom, __CfgItemMetaName__, UnLua::GetState()))
	{
		return pItemRom;
	}	
	return nullptr;
}


Table* FCfgManager::RegisTable(FString& info)
{
	rapidjson::Document doc;
	doc.Parse(TCHAR_TO_UTF8(*info));
	if (doc.IsArray())
	{
		UE_LOG(LogTemp, Error, TEXT("[cfg]FCfgManager::RegisTable error"));
		return nullptr;
	}
	SCfgStructInfo* pStructInfo = new SCfgStructInfo();
	pStructInfo->m_cfgName = FString(doc["name"].GetString());
	pStructInfo->m_nId = m_nTableId++;
	const rapidjson::Value& fields = doc["fields"];
	for (rapidjson::SizeType j = 0; j < fields.Size(); j++)
	{
		const rapidjson::Value& singleField = fields[j];
		SCfgStructItemType itemInfo;
		FString ItemName = singleField["name"].GetString();
		itemInfo.nType = GetCfgType(singleField["type"].GetString());
		if (singleField.HasMember("value"))
			itemInfo.nValue = GetCfgType(singleField["value"].GetString());
		if (singleField.HasMember("key"))
			itemInfo.nKeyType = GetCfgType(singleField["key"].GetString());
		pStructInfo->m_nameToIndex.Add(ItemName, pStructInfo->m_Items.Num());
		pStructInfo->m_Items.Add(itemInfo);
	}
	if (pStructInfo->m_Items.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[cfg]FCfgManager::RegisTable error, field num is 0"))
		return nullptr;
	}
	pStructInfo->m_keyType = pStructInfo->m_Items[0].nType;
	FCfgTable* pCfgTable = new FCfgTable(pStructInfo);
	m_idToTable.Add(pStructInfo->m_nId, pCfgTable);
	pCfgTable->InitTable();
	TArray<uint8> ocFile;
	FString strPath = GetOcFullPath(*pStructInfo->m_cfgName);
	FFileHelper::LoadFileToArray(ocFile, *strPath);
	if(!pCfgTable->GenerateRows(ocFile.GetData(), ocFile.Num()))
	{
		//CLuaMessageTableOptInC::OnlyPrintLuaTrackback("load oc error");
		delete pCfgTable;
		return nullptr;
	}
	Table* pTable = CLuaMessageTableOptInC::CreateNewTable("FLuaCfgInterface", 1, 1, UnLua::GetState(), true);
	m_luatableToCTable.Add(pTable, pCfgTable);
	return pTable;
}

FCfgTable* FCfgManager::GetCfgTable(int id)
{
	return m_idToTable.FindRef(id);
}

FCfgTable* FCfgManager::GetCfgTable(Table* pLuaTable)
{
	return m_luatableToCTable.FindRef(pLuaTable);
}

FString	FCfgManager::GetOcFullPath(const FString& ocName)
{
	FString strOcPath =   FPaths::Combine(UnLua::GetDonwloadResPath(), TEXT("Content/"), m_ocPath,ocName + TEXT(".oc"));
	if (IFileManager::Get().FileExists(*strOcPath))
		return strOcPath;
	strOcPath = FString::Printf(
        TEXT("%s%s%s.oc"), *FPaths::ProjectContentDir(),*m_ocPath, *ocName);
	return strOcPath;
}





