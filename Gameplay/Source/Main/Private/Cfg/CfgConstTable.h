#pragma once
#include "CfgTable.h"

class FCfgConstTable: public FCfgTable
{
public:
	FCfgConstTable(SCfgStructInfo* Info);

	virtual uint8* GetRow(FString& key);
	virtual uint8* GetRow(int64 id);
	virtual  bool   GenerateRows(uint8* pBuffer, uint32 nLen);
};
