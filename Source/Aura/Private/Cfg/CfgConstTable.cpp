#include "CfgConstTable.h"

#include "LuaManger.h"

FCfgConstTable::FCfgConstTable(SCfgStructInfo* Info) : FCfgTable(Info)
{
}

uint8* FCfgConstTable::GetRow(FString& key)
{
	return FCfgTable::GetRow(key);
}

uint8* FCfgConstTable::GetRow(int64 id)
{
	return FCfgTable::GetRow(id);
}

bool FCfgConstTable::GenerateRows(uint8* pBuffer, uint32 nLen)
{
	try
	{
		uint32 nTempSize;
		uint8* pOneRow = GenerateRow(pBuffer, nTempSize, nLen);
		if (m_pIntKeyRows == nullptr)
			m_pIntKeyRows = new TMap<int64, uint8*>();
		m_pIntKeyRows->Add(0, pOneRow);
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("[cfg]FCfgConstTable init oc error"))
		return false;
	}
	return true;
}
