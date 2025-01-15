#include "CfgTable.h"


#include <stdexcept>
#include <string>


#include "CfgManager.h"
#include "EndianOpt.h"
#include "lapi.h"
#include "lstring.h"
#include "ltable.h"
#include "lua.hpp"
#include "LuaManger.h"
#include "LuaMessageTableOptInC.h"
#include "UnLua.h"

#define _CFGSCRIPTESTRUCTPRE_ TEXT("UCfgScriptStruct")

TMap<int, SCfgStructInfo*> FCfgTable::m_idToBeans;
TMap<FString, int> FCfgTable::m_NameToBeanId;

FCfgTable::FCfgTable(SCfgStructInfo* Info)
{
	m_pInfo = Info;
	m_pIntKeyRows = nullptr;
	m_pStrKeyRows = nullptr;
	//m_pStruct = nullptr;
}

FCfgTable::~FCfgTable()
{
	if (m_pIntKeyRows)
	{
		for (auto& item : *m_pIntKeyRows)
		{
			//释放
			delete item.Value;
		}
		m_pIntKeyRows->Empty();
		delete m_pIntKeyRows;
		m_pIntKeyRows = nullptr;
	}
	if (m_pStrKeyRows)
	{
		for (auto& item : *m_pStrKeyRows)
		{
			FMemory::Free(item.Value);
			//delete []item.Value;
		}
		m_pStrKeyRows->Empty();
		delete m_pStrKeyRows;
		m_pStrKeyRows = nullptr;
	}
	if (m_pInfo)
		delete m_pInfo;
	m_pInfo = nullptr;
}


uint8 FCfgTable::GetCfgTypeSize(int ValueType)
{
	uint8 nLen = 0;
	switch (ValueType)
	{
	case CFG_TYPE_BOOL:
		nLen = 1;
		break;
	case CFG_TYPE_INT:
		nLen = 4;
		break;
	case CFG_TYPE_DOUBLE:
		nLen = 8;
		break;
	case CFG_TYPE_FLOAT:
		nLen = 4;
		break;
	default:
		nLen = sizeof(PTRINT);
		break;
	}
	return nLen;
}

void FCfgTable::InitTable()
{
	if (m_pInfo)
	{
		InitCfgStruct(m_pInfo);
		if (m_pInfo->m_keyType == CFG_TYPE_STRING)
		{
			m_pStrKeyRows = new TMap<FString, uint8*>();
		}
		else
		{
			m_pIntKeyRows = new TMap<int64, uint8*>();
		}
	}
}

void FCfgTable::InitCfgStruct(SCfgStructInfo* pStructInfo)
{
	if (pStructInfo == nullptr)
		return;
	pStructInfo->m_size = 0;
	for (int i = 0; i < pStructInfo->m_Items.Num(); i++)
	{
		auto& item = pStructInfo->m_Items[i];
		item.nOffset = pStructInfo->m_size;
		int nBitLen = GetCfgTypeSize(item.nType);
		pStructInfo->m_size += nBitLen;
	}
}

//string
uint8* FCfgTable::Generate(uint8* pBuffer, uint32& nSize, uint32 bufferLen)
{
	uint32 nLen = 0;
	FEndianOpt::GetValueFromBuffer(pBuffer, nLen);
	uint16 bitLen = sizeof(nLen);
	nSize = nLen + bitLen;
	if (nSize > bufferLen || nLen == MAX_uint32)
		throw std::out_of_range("oc error, out range");
	uint8* pMem = (uint8*)FMemory::Malloc(nSize);
	FMemory::Memzero(pMem, nSize);
	FMemory::Memcpy(pMem, &nLen, bitLen);
	FMemory::Memcpy(pMem + bitLen, pBuffer + bitLen, nLen);
	return pMem;
}

/*
A * p = new A();
long n;
memcpy( & n, & p, sizeof(long) );
A * p2;
memcpy( & p2, & n, sizeof(long) );
if ( p2 != p ) {
printf( "FUCK!!!!!!!!!!!!!!!\n" );
}
 */
//list or set
uint8* FCfgTable::Generate(uint8* pBuffer, uint32& nSize, int ValueType, uint32 bufferLen)
{
	nSize = 0;
	uint32 count = 0;
	if (4 > bufferLen)
		throw std::out_of_range("oc error, out range");
	FEndianOpt::GetValueFromBuffer(pBuffer, count);
	nSize += sizeof(count);
	int itemLen = GetCfgTypeSize(ValueType);
	uint32 allSize = sizeof(count) + count * itemLen;
	uint8* pMem = (uint8*)FMemory::Malloc(allSize);
	FMemory::Memzero(pMem, allSize);
	FMemory::Memcpy(pMem, &count, sizeof(count)); //前两位为数量
	uint8* pDest = pMem + sizeof(count);
	if (ValueType == CFG_TYPE_STRING)
	{
		for (uint16 i = 0; i < count; i++)
		{
			uint32 nTempSize = 0;
			uint8* pStr = Generate(pBuffer + nSize, nTempSize, bufferLen - nSize);
			nSize += nTempSize;
			FMemory::Memcpy(pDest, &pStr, itemLen);
			pDest += itemLen;
		}
	}
	else if (ValueType == CFG_TYPE_LIST || ValueType == CFG_TYPE_SET || ValueType == CFG_TYPE_MAP)
	{
		UE_LOG(LogTemp, Error, TEXT("[cfg]FCfgTable GenerateList error, value can not be list,set,map"));
		return nullptr;
	}
	else if (ValueType >= CFG_TYPE_BEAN)
	{
		SCfgStructInfo* otherInfo = m_idToBeans.FindRef(ValueType);
		if (otherInfo == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("[cfg]FCfgTable GenerateBean in list error, bean not exist %d"), ValueType);
			return nullptr;
		}
		for (uint16 i = 0; i < count; i++)
		{
			uint32 nTempSize = 0;
			uint8* pStr = Generate(pBuffer + nSize, nTempSize, otherInfo, bufferLen - nSize);
			nSize += nTempSize;
			FMemory::Memcpy(pDest, &pStr, itemLen);
			pDest += itemLen;
		}
	}
	else
	{
		for (uint16 i = 0; i < count; i++)
		{
			//FMemory::Memcpy(pDest , pBuffer+nSize, itemLen);
			if (nSize + itemLen > bufferLen)
				throw std::out_of_range("oc error, out range");
			FEndianOpt::CopyBuffer(pDest, pBuffer + nSize, itemLen);
			nSize += itemLen;
			pDest += itemLen;
		}
	}

	SCfgItemRow* pListRom = new SCfgItemRow();
	pListRom->m_pData = pMem;
	pListRom->m_beanId = ValueType;

	return (uint8*)pListRom;
}

//map  ：两字节的大小，每一项：key,value, key,value
uint8* FCfgTable::Generate(uint8* pBuffer, uint32& nSize, int ValueType, int keyType, uint32 bufferLen)
{
	nSize = 0;
	uint32 count = 0;
	if (4 > bufferLen)
		throw std::out_of_range("oc error, out range");
	FEndianOpt::GetValueFromBuffer(pBuffer, count);
	nSize += sizeof(count);
	int itemLen = GetCfgTypeSize(ValueType);
	int keyLen = GetCfgTypeSize(keyType);
	uint32 allSize = sizeof(count) + count * (itemLen + keyLen);
	uint8* pMem = (uint8*)FMemory::Malloc(allSize);
	FMemory::Memzero(pMem, allSize);
	FMemory::Memcpy(pMem, &count, sizeof(count)); //前两位为数量
	uint8* pDest = pMem + sizeof(count);
	for (uint16 i = 0; i < count; i++)
	{
		//get key
		if (keyType == CFG_TYPE_INT)
		{
			if (nSize + keyLen > bufferLen)
				throw std::out_of_range("oc error, out range");
			FEndianOpt::CopyBuffer(pDest, pBuffer + nSize, keyLen);
			nSize += keyLen;
		}
		else if (keyType == CFG_TYPE_STRING)
		{
			uint32 nTempSize = 0;
			uint8* pStr = Generate(pBuffer + nSize, nTempSize, bufferLen - nSize);
			FMemory::Memcpy(pDest, &pStr, keyLen);
			nSize += nTempSize;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[cfg]oc %s map key(%d) error"), *m_pInfo->m_cfgName, keyType);
			return nullptr;
		}
		pDest += keyLen;

		if (ValueType < CFG_TYPE_STRING)
		{
			if (nSize + itemLen > bufferLen)
				throw std::out_of_range("oc error, out range");
			FEndianOpt::CopyBuffer(pDest, pBuffer + nSize, itemLen);
			nSize += itemLen;
		}
		else if (ValueType == CFG_TYPE_STRING)
		{
			uint32 nTempSize = 0;
			uint8* pStr = Generate(pBuffer + nSize, nTempSize, bufferLen - nSize);
			FMemory::Memcpy(pDest, &pStr, itemLen);
			nSize += nTempSize;
		}
		else if (ValueType >= CFG_TYPE_BEAN)
		{
			SCfgStructInfo* otherInfo = m_idToBeans.FindRef(ValueType);
			if (otherInfo == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("[cfg]FCfgTable %s map error, bean not exist %d"), *m_pInfo->m_cfgName, ValueType);
				return nullptr;
			}
			uint32 nTempSize = 0;
			uint8* pStr = Generate(pBuffer + nSize, nTempSize, otherInfo, bufferLen - nSize);
			FMemory::Memcpy(pDest, &pStr, itemLen);
			nSize += nTempSize;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[cfg]oc %s map value(%d) error"), *m_pInfo->m_cfgName, ValueType);
			return nullptr;
		}
		pDest += itemLen;
	}

	SCfgMapRow* pMapRow = new SCfgMapRow();
	pMapRow->m_pData = pMem;
	pMapRow->m_beanId = ValueType;
	pMapRow->m_keyType = keyType;

	return (uint8*)pMapRow;
}

uint8* FCfgTable::GenerateBean(uint8* pBuffer, uint32& nSize, int beanId, uint32 bufferLen)
{
	SCfgStructInfo* pBean = m_idToBeans.FindRef(beanId);
	if (pBean == nullptr)
		return nullptr;
	return Generate(pBuffer, nSize, pBean, bufferLen);
}

uint8* FCfgTable::Generate(uint8* pBuffer, uint32& nSize, SCfgStructInfo* pStructInfo, uint32 bufferLen)
{
	if (!pStructInfo || pStructInfo->m_size <= 0)
		return nullptr;
	nSize = 0;
	uint8* pMem = (uint8*)FMemory::Malloc(pStructInfo->m_size);
	FMemory::Memzero(pMem, pStructInfo->m_size);
	uint8* pTempMen = pMem;
	for (int i = 0; i < pStructInfo->m_Items.Num(); i++)
	{
		auto& item = pStructInfo->m_Items[i];
		pTempMen = pMem + item.nOffset;
		int nBitLen = GetCfgTypeSize(item.nType);
		if (item.nType < CFG_TYPE_STRING)
		{
			if (nSize + nBitLen > bufferLen)
				throw std::out_of_range("oc error, out range");
			FEndianOpt::CopyBuffer(pTempMen, pBuffer + nSize, nBitLen);
			nSize += nBitLen;
		}
		else if (item.nType == CFG_TYPE_STRING)
		{
			uint32 nStrSize = 0;
			uint8* pStr = Generate(pBuffer + nSize, nStrSize, bufferLen - nSize);
			FMemory::Memcpy(pTempMen, &pStr, nBitLen);
			nSize += nStrSize;
		}
		else if (item.nType == CFG_TYPE_LIST || item.nType == CFG_TYPE_SET)
		{
			uint32 nListSize = 0;
			uint8* pStr = Generate(pBuffer + nSize, nListSize, item.nValue, bufferLen - nSize);
			FMemory::Memcpy(pTempMen, &pStr, nBitLen);
			nSize += nListSize;
		}
		else if (item.nType == CFG_TYPE_MAP)
		{
			uint32 nListSize = 0;
			uint8* pStr = Generate(pBuffer + nSize, nListSize, item.nValue, item.nKeyType, bufferLen - nSize);
			FMemory::Memcpy(pTempMen, &pStr, nBitLen);
			nSize += nListSize;
		}
		else
		{
			SCfgStructInfo* otherInfo = m_idToBeans.FindRef(item.nType);
			if (otherInfo == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("[cfg]FCfgTable GenerateBean error, bean not exist %d"), item.nType);
				return nullptr;
			}
			uint32 nTempSize = 0;
			uint8* pStr = Generate(pBuffer + nSize, nTempSize, otherInfo, bufferLen - nSize);
			FMemory::Memcpy(pTempMen, &pStr, nBitLen);
			nSize += nTempSize;
		}
	}

	SCfgItemRow* pItemRom = new SCfgItemRow();
	pItemRom->m_pData = pMem;
	pItemRom->m_beanId = pStructInfo->m_nId;

	return (uint8*)pItemRom;
}

uint8* FCfgTable::GenerateRow(uint8* pBuffer, uint32& nSize, uint32 bufferLen)
{
	if (!m_pInfo || m_pInfo->m_size <= 0)
		return nullptr;
	nSize = 0;
	return Generate(pBuffer, nSize, m_pInfo, bufferLen);
}

bool FCfgTable::GenerateRows(uint8* pBuffer, uint32 nLen)
{
	if (m_pInfo == nullptr)
		return false;
	if (m_pInfo->m_keyType != CFG_TYPE_INT &&
		m_pInfo->m_keyType != CFG_TYPE_STRING)
	{
		UE_LOG(LogTemp, Error, TEXT("[cfg]oc error,key type(%d) invalid in table( %s)"), m_pInfo->m_keyType, *m_pInfo->m_cfgName);
		return false;
	}
	try
	{
		uint32 nSize = 0;
		while (nSize < nLen)
		{
			uint32 nTempSize = 0;
			uint8* pOneRow = GenerateRow(pBuffer + nSize, nTempSize, nLen - nSize);
			if (pOneRow == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("[cfg]oc error,table :%s"), *m_pInfo->m_cfgName);
				return false;
			}
			//get key
			if (m_pInfo->m_keyType == CFG_TYPE_INT)
			{
				int nKey = 0;
				FEndianOpt::GetValueFromBuffer(pBuffer + nSize, nKey);
				m_pIntKeyRows->Add(nKey, pOneRow);
			}
			else if (m_pInfo->m_keyType == CFG_TYPE_STRING)
			{
				uint32 nStrLen = 0;
				FEndianOpt::GetValueFromBuffer(pBuffer + nSize, nStrLen);
				TArray<uint8> str(pBuffer + nSize + 4, nStrLen);
				str.Add('\0');
				FString strKey(UTF8_TO_TCHAR(str.GetData()));
				m_pStrKeyRows->Add(strKey, pOneRow);
			}

			nSize += nTempSize;
		}
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("[cfg]FCfgTable load oc error %s"), *m_pInfo->m_cfgName)
		return false;
	}
	return true;
}

uint8* FCfgTable::GetRow(FString& key)
{
	if (m_pStrKeyRows == nullptr)
		return nullptr;
	return m_pStrKeyRows->FindRef(key);
}

uint8* FCfgTable::GetRow(int64 id)
{
	if (m_pIntKeyRows == nullptr)
		return nullptr;
	uint8** pRet = m_pIntKeyRows->Find(id);
	if (pRet != nullptr)
		return *pRet;
	else
		return nullptr;
}

int FCfgTable::GetRowsCount()
{
	if (m_pIntKeyRows != nullptr)
		return m_pIntKeyRows->Num();
	if (m_pStrKeyRows != nullptr)
		return m_pStrKeyRows->Num();
	return 0;
}

int FCfgTable::IterTable(lua_State* L)
{
	if (m_pInfo == nullptr)
		return 0;
	if (m_pInfo->m_keyType == CFG_TYPE_INT)
	{
		int64* pId = nullptr;
		return IterTable(pId, L);
	}
	if (m_pInfo->m_keyType == CFG_TYPE_STRING)
	{
		const char* pStr = nullptr;
		return IterTable(pStr, L);
	}
	return 0;
}

int FCfgTable::IterTable(int64* key, lua_State* L)
{
	if (m_pIntKeyRows == nullptr || m_pIntKeyRows->Num() == 0)
		return 0;
	bool bFind = false;
	for (auto& item : *m_pIntKeyRows)
	{
		if (key == nullptr || bFind)
		{
			lua_pushinteger(L, item.Key);
			SCfgItemRow* pItemRom = (SCfgItemRow*)item.Value;
			if (PushItemRowToLua(pItemRom, __CfgItemMetaName__, L))
				return 2;
			else
				return 1;
		}
		else if (item.Key == *key)
		{
			bFind = true;
		}
	}
	return 0;
}

int FCfgTable::IterTable(const char* key, lua_State* L)
{
	if (m_pStrKeyRows == nullptr || m_pStrKeyRows->Num() == 0)
		return 0;

	bool bFind = false;
	for (auto& item : *m_pStrKeyRows)
	{
		if (key == nullptr || bFind)
		{
			lua_pushstring(L, TCHAR_TO_UTF8(*item.Key));
			SCfgItemRow* pItemRom = (SCfgItemRow*)item.Value;
			if (PushItemRowToLua(pItemRom, __CfgItemMetaName__, L))
				return 2;
			else
				return 1;
		}
		if (item.Key == UTF8_TO_TCHAR(key))
		{
			bFind = true;
		}
	}
	return 0;
}

bool FCfgTable::GetColumnByName(SCfgItemRow* pItem, FString& strName, lua_State* L)
{
	if (m_pInfo == nullptr || pItem == nullptr)
		return false;
	int* pId = m_pInfo->m_nameToIndex.Find(strName);
	if (pId == nullptr || *pId < 0 || *pId >= m_pInfo->m_Items.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("[cfg]FCfgTable::GetColumnByName Column(%s) not exist in %s"), *strName, *m_pInfo->m_cfgName)
		return false;
	}
	auto& subitem = m_pInfo->m_Items[*pId];
	return PushSubItemToLua(pItem->m_pData, subitem.nOffset, subitem.nType, L);
}

bool FCfgTable::GetBeanColumnByName(SCfgItemRow* Item, FString& strName, lua_State* L)
{
	if (Item == nullptr)
		return false;
	SCfgStructInfo* pBean = m_idToBeans.FindRef(Item->m_beanId);
	if (pBean == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[cfg]GetBeanColumnByName error, id=%d"), Item->m_beanId)
		return false;
	}
	int* pId = pBean->m_nameToIndex.Find(strName);
	if (pId == nullptr || *pId < 0 || *pId >= pBean->m_Items.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("[cfg]FCfgTable::GetBeanColumnByName Column(%s) not exist in %s"), *strName, *pBean->m_cfgName)
		return false;
	}
	auto& subitem = pBean->m_Items[*pId];
	return PushSubItemToLua(Item->m_pData, subitem.nOffset, subitem.nType, L);
}

bool FCfgTable::GetTValueFromBuffer(TValue* pValue, uint8* pBuffer, int ValueType)
{
	if (pValue == nullptr || pBuffer == nullptr)
		return false;
	int nBitLen = GetCfgTypeSize(ValueType);
	switch (ValueType)
	{
	case CFG_TYPE_BOOL:
		{
			int8 n = *pBuffer;
			CLuaMessageTableOptInC::SetBoolValue(pValue, n > 0);
			break;
		}
	case CFG_TYPE_INT:
		{
			int32 n = 0;
			FMemory::Memcpy(&n, pBuffer, nBitLen);
			setivalue(pValue, n);
			break;
		}
	case CFG_TYPE_DOUBLE:
		{
			double n = 0;
			FMemory::Memcpy(&n, pBuffer, nBitLen);
			setfltvalue(pValue, n);
			break;
		}
	case CFG_TYPE_FLOAT:
		{
			float f = 0;
			FMemory::Memcpy(&f, pBuffer, nBitLen);
			setfltvalue(pValue, f);
			break;
		}
	case CFG_TYPE_STRING:
		{
			uint8* pStrAddress;
			FMemory::Memcpy(&pStrAddress, pBuffer, nBitLen);
			uint32 nLen = 0;
			FMemory::Memcpy(&nLen, pStrAddress, sizeof(nLen));
			lua_State* L = UnLua::GetState();
			TString* toLuaStr = luaS_newlstr(L, (char*)pStrAddress + sizeof(nLen), nLen);
			setsvalue(L, pValue, toLuaStr);

			break;
		}
	case CFG_TYPE_LIST:
	case CFG_TYPE_SET:
	case CFG_TYPE_MAP:
		UE_LOG(LogTemp, Warning, TEXT("[cfg]GetTValueFromBuffer kay value error:%d"), ValueType);
		return false;
	default:
		{
			SCfgItemRow* pItemRom;
			FMemory::Memcpy(&pItemRom, pBuffer, nBitLen);
			if (pItemRom == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("[cfg]GetTValueFromBuffer kay value error:%d"), ValueType);
				return false;
			}

			lua_State* L = UnLua::GetState();
			if (pItemRom->m_luaUd == nullptr)
			{
				Udata* ud = CLuaMessageTableOptInC::CreateUsedata(sizeof(void*), 0, pItemRom, L);
				int Type = lua_getglobal(L, __CfgBeanMetaName__);
				if (Type == LUA_TTABLE)
				{
					lua_setmetatable(L, -2);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("cfg rom setmetabatle error %s: %d"), UTF8_TO_TCHAR(__CfgBeanMetaName__), Type)
				}
				pItemRom->m_luaUd = ud;
				lua_pop(L, 1);
			}
			setuvalue(L, pValue, pItemRom->m_luaUd);

			break;
		}
	}

	return true;
}

bool FCfgTable::PushSubItemToLua(uint8* row, int offset, int type, lua_State* L)
{
	if (row == nullptr || offset < 0 || type < 0)
		return false;
	uint8* pSubItem = row + offset;
	int nBitLen = GetCfgTypeSize(type);
	switch (type)
	{
	case CFG_TYPE_BOOL:
		lua_pushboolean(L, *pSubItem);
		break;
	case CFG_TYPE_INT:
		{
			int32 n = 0;
			FMemory::Memcpy(&n, pSubItem, nBitLen);
			lua_pushinteger(L, n);
			break;
		}
	case CFG_TYPE_DOUBLE:
		{
			double n = 0;
			FMemory::Memcpy(&n, pSubItem, nBitLen);
			lua_pushnumber(L, n);
			break;
		}
	case CFG_TYPE_FLOAT:
		{
			float f = 0;
			FMemory::Memcpy(&f, pSubItem, nBitLen);
			lua_pushnumber(L, f);
			break;
		}
	case CFG_TYPE_STRING:
		{
			uint8* pStrAddress;
			FMemory::Memcpy(&pStrAddress, pSubItem, nBitLen);
			uint32 nLen = 0;
			FMemory::Memcpy(&nLen, pStrAddress, sizeof(nLen));
			lua_pushlstring(L, (char*)pStrAddress + sizeof(nLen), nLen);
			break;
		}
	case CFG_TYPE_LIST:
		{
			SCfgItemRow* pItemRom;
			FMemory::Memcpy(&pItemRom, pSubItem, nBitLen);
			if (pItemRom == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("[cfg]PushSubItemToLua list null"))
				return false;
			}
			int listCount = GetListSize(pItemRom);

			Table* pSubTabel = CLuaMessageTableOptInC::CreateNewTable("ArrayList", 1, 2, L, true);
			Table* pDataTable = CLuaMessageTableOptInC::CreateNewTable(nullptr, listCount, 1);
			CLuaMessageTableOptInC::AddTable(pSubTabel, "_data", pDataTable);
			CLuaMessageTableOptInC::AddValue(pSubTabel, "_size", listCount);
			int nValueLen = GetCfgTypeSize(pItemRom->m_beanId);
			uint8* pTempBuffer = pItemRom->m_pData + 4;
			for (int index = 1; index <= listCount; index++)
			{
				TValue uValue;
				if (GetTValueFromBuffer(&uValue, pTempBuffer, pItemRom->m_beanId))
				{
					CLuaMessageTableOptInC::AddTValue(pDataTable, index, &uValue);
				}
				pTempBuffer += nValueLen;
			}

			return true;
		}
	case CFG_TYPE_SET:
		{
			SCfgItemRow* pItemRom;
			FMemory::Memcpy(&pItemRom, pSubItem, nBitLen);
			if (pItemRom == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("[cfg]PushSubItemToLua set null"))
				return false;
			}
			int listCount = GetListSize(pItemRom);

			Table* pSubTabel = CLuaMessageTableOptInC::CreateNewTable("HashSet", 1, 2, L, true);
			Table* pDataTable = CLuaMessageTableOptInC::CreateNewTable(nullptr, listCount, 1);
			CLuaMessageTableOptInC::AddTable(pSubTabel, "_data", pDataTable);
			CLuaMessageTableOptInC::AddValue(pSubTabel, "_size", listCount);
			int nValueLen = GetCfgTypeSize(pItemRom->m_beanId);
			uint8* pTempBuffer = pItemRom->m_pData + 4;
			for (int index = 1; index <= listCount; index++)
			{
				TValue key;
				if (GetTValueFromBuffer(&key, pTempBuffer, pItemRom->m_beanId))
				{
					TValue* pNewValue = luaH_set(L, pDataTable, &key);
					setobj(L, pNewValue, &key);
				}
				pTempBuffer += nValueLen;
			}
			return true;
		}
	case CFG_TYPE_MAP:
		{
			SCfgMapRow* pItemRom;
			FMemory::Memcpy(&pItemRom, pSubItem, nBitLen);
			if (pItemRom == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("[cfg]PushSubItemToLua map null"))
				return false;
			}
			int listCount = GetListSize(pItemRom);

			Table* pSubTabel = CLuaMessageTableOptInC::CreateNewTable("HashMap", 1, 2, L, true);
			Table* pDataTable = CLuaMessageTableOptInC::CreateNewTable(nullptr, listCount, 1);
			CLuaMessageTableOptInC::AddTable(pSubTabel, "_data", pDataTable);
			CLuaMessageTableOptInC::AddValue(pSubTabel, "_size", listCount);
			int nValueLen = GetCfgTypeSize(pItemRom->m_beanId);
			int nKeylen = GetCfgTypeSize(pItemRom->m_keyType);
			uint8* pTempBuffer = pItemRom->m_pData + 4;
			for (int index = 1; index <= listCount; index++)
			{
				TValue key;
				TValue uValue;
				if (GetTValueFromBuffer(&uValue, pTempBuffer + nKeylen, pItemRom->m_beanId) &&
					GetTValueFromBuffer(&key, pTempBuffer, pItemRom->m_keyType))
				{
					TValue* pNewValue = luaH_set(L, pDataTable, &key);
					setobj(L, pNewValue, &uValue);
				}
				pTempBuffer += nValueLen + nKeylen;
			}
			return true;
		}
	default:
		{
			SCfgItemRow* pItemRom;
			FMemory::Memcpy(&pItemRom, pSubItem, nBitLen);
			return PushItemRowToLua(pItemRom,__CfgBeanMetaName__, L);
		}
	}

	return true;
}

bool FCfgTable::PushItemRowToLua(SCfgItemRow* pItemRom, const char* metaName, lua_State* L)
{
	if (pItemRom == nullptr || metaName == nullptr)
	{
		return false;
	}
	if (pItemRom->m_luaUd == nullptr)
	{
		Udata* ud = CLuaMessageTableOptInC::CreateUsedata(sizeof(void*), 0, pItemRom, L);
		int Type = lua_getglobal(L, metaName);
		if (Type == LUA_TTABLE)
		{
			lua_setmetatable(L, -2);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("cfg item setmetabatle error %s: %d"), UTF8_TO_TCHAR(metaName), Type)
		}
		pItemRom->m_luaUd = ud;
	}
	else
	{
		CLuaMessageTableOptInC::PushUserdata(pItemRom->m_luaUd, L);
	}
	return true;
}

int FCfgTable::GetListSize(SCfgItemRow* pListRow)
{
	if (pListRow == nullptr)
		return 0;
	uint32 nCount = 0;
	FMemory::Memcpy(&nCount, pListRow->m_pData, sizeof(nCount));
	return nCount;
}

//nIndex [1,max]
bool FCfgTable::GetListSubItem(SCfgItemRow* pListRow, int nIndex, lua_State* L)
{
	if (pListRow == nullptr || nIndex < 1)
		return false;
	int nMaxCount = GetListSize(pListRow);
	if (nIndex > nMaxCount)
		return false;
	int nBitLen = GetCfgTypeSize(pListRow->m_beanId);
	nIndex--;
	return PushSubItemToLua(pListRow->m_pData + 4, nIndex * nBitLen, pListRow->m_beanId, L);
}

int FCfgTable::CheckSetContain(SCfgItemRow* Item, uint64& nValue)
{
	if (Item == nullptr)
		return 0;
	if (Item->m_beanId != CFG_TYPE_INT)
		return 0;
	int nMaxCount = GetListSize(Item);
	int nValueLen = GetCfgTypeSize(Item->m_beanId);
	uint8* pMem = Item->m_pData + 4;
	for (int i = 0; i < nMaxCount; i++)
	{
		int nId = 0;
		FMemory::Memcpy(&nId, pMem, nValueLen);
		if (nId == nValue)
			return 1;
		pMem += nValueLen;
	}
	return 0;
}

int FCfgTable::CheckSetContain(SCfgItemRow* Item, const char* pStr)
{
	if (Item == nullptr)
		return 0;
	if (Item->m_beanId != CFG_TYPE_STRING)
		return 0;
	int nMaxCount = GetListSize(Item);
	int nValueLen = GetCfgTypeSize(Item->m_beanId);
	uint8* pMem = Item->m_pData + 4;
	for (int i = 0; i < nMaxCount; i++)
	{
		uint8* pStrAddress;
		FMemory::Memcpy(&pStrAddress, pMem, nValueLen);
		uint32 nLen = 0;
		FMemory::Memcpy(&nLen, pStrAddress, sizeof(nLen));
		if (strncmp(pStr, (char*)pStrAddress + sizeof(nLen), nLen) == 0)
			return 1;
		pMem += nValueLen;
	}
	return 0;
}

bool FCfgTable::GetMapItemByKey(SCfgMapRow* pItem, uint64 nKeyId, lua_State* L)
{
	if (pItem == nullptr)
		return false;
	if (pItem->m_keyType != CFG_TYPE_INT)
		return false;
	int nMaxCount = GetListSize(pItem);
	if (nMaxCount == 0)
		return false;
	int itemLen = GetCfgTypeSize(pItem->m_beanId);
	int keyLen = GetCfgTypeSize(pItem->m_keyType);
	uint8* pMem = pItem->m_pData + 4;
	uint64 nGetId = 0;
	for (int i = 0; i < nMaxCount; i++)
	{
		if (pItem->m_keyType == CFG_TYPE_INT)
		{
			uint32 nId = 0;
			FMemory::Memcpy(&nId, pMem, 4);
			nGetId = nId;
		}
		else
		{
			FMemory::Memcpy(&nGetId, pMem, 8);
		}
		if (nGetId == nKeyId)
			return PushSubItemToLua(pMem, keyLen, pItem->m_beanId, L);
		pMem = pMem + itemLen + keyLen;
	}
	return false;
}

bool FCfgTable::GetMapItemByKey(SCfgMapRow* pItem, const char* pKey, lua_State* L)
{
	if (pItem == nullptr)
		return false;
	if (pItem->m_keyType != CFG_TYPE_STRING)
		return false;
	int nKeyParamLen = strlen(pKey);
	if (nKeyParamLen == 0)
		return false;
	int nMaxCount = GetListSize(pItem);
	if (nMaxCount == 0)
		return false;
	int itemLen = GetCfgTypeSize(pItem->m_beanId);
	int keyLen = GetCfgTypeSize(pItem->m_keyType);
	uint8* pMem = pItem->m_pData + 4;
	for (int i = 0; i < nMaxCount; i++)
	{
		uint8* pStrAddress;
		FMemory::Memcpy(&pStrAddress, pMem, keyLen);
		uint32 nLen = 0;
		FMemory::Memcpy(&nLen, pStrAddress, sizeof(nLen));
		if (nKeyParamLen == nLen && strncmp(pKey, (char*)pStrAddress + sizeof(nLen), nLen) == 0)
			return PushSubItemToLua(pMem, keyLen, pItem->m_beanId, L);
		pMem = pMem + itemLen + keyLen;
	}
	return false;
}

bool FCfgTable::GetMapItemByIndex(SCfgMapRow* pItem, uint32 nIndex, lua_State* L)
{
	if (pItem == nullptr)
		return false;
	uint32 nMaxCount = GetListSize(pItem);
	if (nMaxCount == 0)
		return false;
	if (nIndex < 1 || nIndex > nMaxCount)
	{
		UE_LOG(LogTemp, Error, TEXT("[cfg]GetMapItemByIndex error, index %d out range[1,%d]"), nIndex, nMaxCount);
		return false;
	}
	int itemLen = GetCfgTypeSize(pItem->m_beanId);
	int keyLen = GetCfgTypeSize(pItem->m_keyType);
	uint8* pMem = pItem->m_pData + 4 + (nIndex - 1) * (itemLen + keyLen);
	return PushSubItemToLua(pMem, keyLen, pItem->m_beanId, L);
}

FString FCfgTable::GetTableName()
{
	if (m_pInfo)
		return m_pInfo->m_cfgName;
	return FString();
}
