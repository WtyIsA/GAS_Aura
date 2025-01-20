#pragma once

struct Table;
struct TString;
struct Udata;
union Node;
struct GCObject;
struct LClosure;
struct CClosure;
struct TValue;
class FLuaMemoryInfo
{
	enum RelationshipType { 
		Relationship_Key_Str = 1,
		Relationship_Key_Number,
		Relationship_Key_Table,
		Relationship_Metatable,
		Relationship_UpValue,
	 };
public:
	struct FLuaTableRef
	{
	    FLuaTableRef(FString strKey, void* parent, bool bHasNext)
    	{
    		key = strKey;
    		pParent = parent;
	    	hasNext = bHasNext;
    	}
		FString key;
		void*	pParent;
		bool	hasNext;
	};

	struct FLuaTableMemInfo
	{
		FLuaTableMemInfo()
		{
			nTimes = 0;
			nMaxSize = 0;
			nTotalSize = 0;
		}
		int nTimes;
		int nMaxSize;
		int nTotalSize;
	};
	FLuaMemoryInfo(){}
	~FLuaMemoryInfo();

	void AnalyzeGcObj();
	void SaveResult(FString savePath);
	static FString GetNameFromNode(Node* pNode);
	
	void GetObjRelation();
	void GetObjRelation(FString strPanelName);
	void SaveRelationship(FString savePath);
private:
	void GetObjRelation(Table* pTable, void* pParent);
	void GetObjRelation(LClosure* plClosure, void* pParent);
	void GetObjRelation(Udata* pUdata);
	void GetObjRelation(CClosure* pCClosure);
	void GetObjRelation(TValue* pValue,Table* pParentTable, RelationshipType type, const char* key, int numKey, const char* key2, FString parentTableName);
	void ObjectRelationshipReport( void *parent, void *child, RelationshipType type, const char *key, int numKey, const char *key2, FString tableName=FString());
	
	void GetParentPaths(const TArray<FLuaTableRef*>&parents, FString key, TArray<FString>& getpath, TArray<void*>&hadAddParent);
	
    void AnalyzeGcObj(GCObject* gcHeader);
	void AnalyzeObj(Udata* pTable);
	void AnalyzeObj(Table* pTable);
	void AnalyzeObj(LClosure* plClosure,int state);
	void GetGlobalTables();
	
	void GetAllLoadModule();

	FString GetTableName(Table* obj);

	TMap<void* ,FString>			GlobalTableToName;
	TMap<void* ,FString>			LoadModuleTableToName;
	TMap<void*, TArray<FLuaTableRef*>>	LuaObjRefInfo; //void*为Table*
	TMap<void*,FString>				FunToName;
	TMap<void*,bool>				HadAnalyzeObj;
	TArray<TString*>				LuaAllTStrings;	//所有的字符串
	TMap<Table*, FLuaTableMemInfo>			MetaTableUseTimes;//对应对应有多少个表对象,void为
	TMap<FString, TArray<void*>>			LuaFunctionUserTimes;
	FString							StrUIPanelName;
};
