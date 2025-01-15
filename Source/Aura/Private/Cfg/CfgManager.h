#pragma once

#include "CfgTable.h"
class FCfgTable;
struct Table;
class FCfgManager
{
	FCfgManager();
	static FCfgManager* m_Ins;
	void Init();
	void UnInit();
public:
	static FCfgManager* Get();
	static void Free();

	bool RegisBean(FString& Info);
	Table* RegisTable(FString& Info);
	SCfgItemRow* RegisConstTable(FString& Info);
	
	
	FCfgTable* GetCfgTable(int id);
	FCfgTable* GetCfgTable(Table* pLuaTable);

	static  int GetCfgType(const char* str);

	void		SetOcPath(const FString& ocPath) { m_ocPath = ocPath;};
private:
	FString		GetOcFullPath(const FString& ocName);
	TMap<int, FCfgTable*>		m_idToTable;
	TMap<Table*, FCfgTable*>	m_luatableToCTable;
	uint16						m_nBeanId;
	uint16						m_nTableId;
	FString						m_ocPath;
};