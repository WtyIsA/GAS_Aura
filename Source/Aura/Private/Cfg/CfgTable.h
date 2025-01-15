#pragma once

#define CFG_TYPE_BOOL 1
#define CFG_TYPE_INT  2
#define CFG_TYPE_DOUBLE		3
#define CFG_TYPE_FLOAT 4
#define CFG_TYPE_STRING 5
#define CFG_TYPE_LIST	6
#define CFG_TYPE_SET	7
#define CFG_TYPE_MAP	8
#define CFG_TYPE_BEAN  20
#define CFG_TYPE_TABLE  10000

#define __CfgItemMetaName__ "FLuaCfgItem"
#define __CfgListMetaName__ "FLuaCfgListItem"
#define __CfgSetMetaName__ "FLuaCfgSetItem"
#define __CfgMapMetaName__ "FLuaCfgMapItem"
#define __CfgBeanMetaName__ "FLuaCfgBeanItem"

struct SCfgStructItemType
{
	SCfgStructItemType()
	{
		nType = 0;
		nValue = 0;
		nOffset = 0;
		nKeyType = 0;
	}
	SCfgStructItemType(const SCfgStructItemType& other)
	{
		nType = other.nType;
		nValue = other.nValue;
		nOffset = other.nOffset;
		nKeyType = other.nKeyType;
	}
	int     nType;
	int		nValue; //主要用于list对应的value
	uint16		nOffset;
	uint16      nKeyType; //map里的key类型
};

struct SCfgStructInfo
{
	SCfgStructInfo()
	{
		m_nId = 0;
		m_keyType = 0;
		m_size = 0;
	}	
	FString m_cfgName;
	int		m_nId;
	uint8   m_keyType;
	uint16	m_size; 
	TArray<SCfgStructItemType>   m_Items;
	TMap<FString, int> m_nameToIndex;
};

struct UCfgScriptStruct;
struct Udata;
struct TValue;
struct SCfgItemRow
{
	SCfgItemRow()
	{
		m_luaUd = nullptr;
		m_pData = nullptr;
		m_beanId = 0;
	}
	Udata * m_luaUd;
	uint8*  m_pData;
	uint16  m_beanId;//表ID，或者list里的beanid或者基本类型ID
};

struct SCfgMapRow : public SCfgItemRow
{
	SCfgMapRow()
	{
		m_keyType = 0;
	}
	uint16 m_keyType;
};
struct lua_State;
class FCfgTable
{
public:
	FCfgTable(SCfgStructInfo* Info);
	virtual ~FCfgTable();
	void InitTable();
	SCfgStructInfo*	GetStructInfo(){ return  m_pInfo;}
	uint8* GenerateRow(uint8* pBuffer,uint32& nSize, uint32 bufferLen);

	virtual uint8* GetRow(FString& key);
	virtual uint8* GetRow(int64 id);
	virtual  bool   GenerateRows(uint8* pBuffer, uint32 nLen);
	
	static uint8  GetCfgTypeSize(int ValueType);
	bool   GetColumnByName(SCfgItemRow *Item, FString& strName, lua_State*L);

	int   IterTable(lua_State*L);
	int   IterTable(int64* key, lua_State*L);
	int   IterTable(const char* key, lua_State*L);
	int   GetRowsCount();

	static bool PushItemRowToLua(SCfgItemRow *Item, const char* metaName, lua_State*L);
	static int GetListSize(SCfgItemRow *pListRow);
	static bool GetListSubItem(SCfgItemRow *pListRow, int nIndex, lua_State*L);
	static bool GetBeanColumnByName(SCfgItemRow *Item, FString& strName, lua_State*L);
	static void InitCfgStruct(SCfgStructInfo* pStructInfo);

	static int CheckSetContain(SCfgItemRow *Item, uint64& nValue);
	static int CheckSetContain(SCfgItemRow *Item, const char* pStr);
	static bool GetMapItemByKey(SCfgMapRow* pItem, uint64 nId, lua_State*L);
	static bool GetMapItemByKey(SCfgMapRow* pItem, const char* pKey, lua_State*L);
	static bool GetMapItemByIndex(SCfgMapRow* pItem, uint32 nIndex, lua_State*L);

	FString GetTableName();
protected:

	uint8* Generate(uint8* pBuffer,uint32& nSize, uint32 bufferLen);  //string
	uint8* Generate(uint8* pBuffer,uint32& nSize, int ValueType, uint32 bufferLen); //list or set
	uint8* Generate(uint8* pBuffer,uint32& nSize, int ValueType, int keyType, uint32 bufferLen); //map
	uint8* GenerateBean(uint8* pBuffer,uint32& nSize, int beanId, uint32 bufferLen);
	uint8* Generate(uint8* pBuffer,uint32& nSize, SCfgStructInfo* pStructInfo, uint32 bufferLen);
	static bool   PushSubItemToLua(uint8* row, int offset, int type, lua_State*L);
	static bool   GetTValueFromBuffer(TValue* pValue, uint8* pBuffer, int ValueType); 
protected:
	SCfgStructInfo*		m_pInfo;

	TMap<int64, uint8*> *   m_pIntKeyRows;
	TMap<FString, uint8*> *m_pStrKeyRows;
public:
	
	static  TMap<int, SCfgStructInfo*>  m_idToBeans;
	static  TMap<FString, int>			m_NameToBeanId;
};