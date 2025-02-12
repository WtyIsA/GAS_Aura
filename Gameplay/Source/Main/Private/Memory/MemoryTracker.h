#pragma once
#if ENABLE_MEMORYTRACKER
struct FMemoryStatsCallstack
{
	/** Separator. */
	static const TCHAR* CallstackSeparator;

	/** Encodes decoded callstack a string, to be like '45+656+6565'. */
	static FString Encode( const TArray<FName>& Callstack )
	{
		FString Result;
		for (const auto& Name : Callstack)
		{
			Result += TTypeToString<uint32>::ToString(Name.GetComparisonIndex().ToUnstableInt() );
			Result += CallstackSeparator;
		}
		return Result;
	}

	/** Decodes encoded callstack to an array of FNames. */
	static void DecodeToNames( const FName& EncodedCallstack, TArray<FName>& out_DecodedCallstack )
	{
		TArray<FString> DecodedCallstack;
		DecodeToStrings( EncodedCallstack, DecodedCallstack );

		// Convert back to FNames
		for (const auto& It : DecodedCallstack)
		{
			uint32 NameInt = 0;
			TTypeFromString<uint32>::FromString(NameInt, *It );
			FNameEntryId Id = FNameEntryId::FromUnstableInt(NameInt);
			const FName LongName = FName(Id, Id, 0 );

			out_DecodedCallstack.Add( LongName );
		}
	}

	/** Converts the encoded callstack into human readable callstack. */
	static FString GetHumanReadable( const FName& EncodedCallstack )
	{
		TArray<FName> DecodedCallstack;
		DecodeToNames( EncodedCallstack, DecodedCallstack );
		const FString Result = GetHumanReadable( DecodedCallstack );
		return Result;
	}

	/** Converts the encoded callstack into human readable callstack. */
	static FString GetHumanReadable( const TArray<FName>& DecodedCallstack )
	{
		FString Result;

		const int32 NumEntries = DecodedCallstack.Num();
		//for (int32 Index = DecodedCallstack.Num() - 1; Index >= 0; --Index)
		for (int32 Index = 0; Index < NumEntries; ++Index)
		{
			const FName LongName = DecodedCallstack[Index];
			const FString ShortName = FStatNameAndInfo::GetShortNameFrom( LongName ).ToString();
			//const FString Group = FStatNameAndInfo::GetGroupNameFrom( LongName ).ToString();
			FString Desc = FStatNameAndInfo::GetDescriptionFrom( LongName );
			Desc.TrimStartInline();

			if (Desc.Len() == 0)
			{
				Result += ShortName;
			}
			else
			{
				Result += Desc;
			}

			if (Index != NumEntries - 1)
			{
				Result += TEXT( " -> " );
			}
		}

		Result.ReplaceInline( TEXT( "STAT_" ), TEXT( "" ), ESearchCase::CaseSensitive );
		return Result;
	}

protected:
	/** Decodes encoded callstack to an array of strings. Where each string is the index of the FName. */
	static void DecodeToStrings( const FName& EncodedCallstack, TArray<FString>& out_DecodedCallstack )
	{
		EncodedCallstack.ToString().ParseIntoArray( out_DecodedCallstack, CallstackSeparator, true );
	}
};

struct FMemoryInfo
{
	FMemoryInfo(uint64 nTag,	int64   nSize, int64 time, const	FString & strEncodedCallstack)
	{
		Tag = nTag;
		Size = nSize;
		Time = time;
		EncodedCallstack = strEncodedCallstack;
	}
	FMemoryInfo(const FMemoryInfo& other)
	{
		Tag = other.Tag;
		Size = other.Size;
		Time = other.Time;
		EncodedCallstack = other.EncodedCallstack;
	}
	FMemoryInfo(FMemoryInfo&& other)
	{
		Tag = other.Tag;
		Size = other.Size;
		Time = other.Time;
		EncodedCallstack = other.EncodedCallstack;
	}
	uint64 Tag;
	int64   Size;
	int64	Time;
	FString EncodedCallstack;
};


class FMemoryTracker
{
	struct FCallstackTrack
	{
		FCallstackTrack()
		{
			FMemory::Memzero(this, sizeof(FCallstackTrack));
		}
		static constexpr int32 Depth = 32;
		uint64 CallStack[Depth];		
		uint64 Size;
		uint32 Count;
		uint32 CachedHash;
		uint64 Tag;

		bool operator==(const FCallstackTrack& Other) const
		{
			bool bEqual = true;
			for (int32 i = 0; i < Depth; ++i)
			{
				if (CallStack[i] != Other.CallStack[i])
				{
					bEqual = false;
					break;
				}
			}
			return bEqual;
		}

		bool operator!=(const FCallstackTrack& Other) const
		{
			return !(*this == Other);
		}
		
		uint32 GetHash() 
		{
			CachedHash = FCrc::MemCrc32(CallStack, sizeof(CallStack), 0);
			return CachedHash;
		}
	};
	
	FMemoryTracker();
	
public:
	static FMemoryTracker* Get();
	static void Free();
	void Init();
	bool SetTracker(bool bTracker, int nIngore, int nStackNum);
	void AddTag(const FString& strTag);
	void AddTag(uint64 nTag);
	void RemoveTag(const FString& strTag);
	void RemoveTag(uint64 nTag);
	void Clear();
	
	void OnAlloc(const void* ptr,  int64 size, uint64 tag);
	void OnFree(const void* ptr,  int64 size, uint64 tag);

	void SaveTag(uint64 tag, const FString& strSavePath);
	void SaveTag(const FString& strTag, const FString& strSavePath);
private:
	static FMemoryTracker* m_Ins;
	TMap<const void*, FMemoryInfo>	MemoryInfo;
	TArray<uint64>					TrackerTags;
	bool		BShouldTracker;
	int			NumIngore;
	int			NumStackList;


	/** Stop tracking a callstack */
	void AddCallstack(FCallstackTrack& Callstack);
	void RemoveCallstack(FCallstackTrack& Callstack);

	FCriticalSection MemCritical;
	bool			bRecursive;
	/** List of all currently allocated pointers */
	TMap<const void*, FCallstackTrack> OpenPointers;

	/** List of all unique callstacks with allocated memory */
	TMap<uint32, FCallstackTrack> UniqueCallstacks;

	/** Set of callstacks that are known to delete memory (not reset on ClearData()) */
	TSet<uint32>	KnownDeleters;

	/** Set of callstacks that are known to resize memory (not reset on ClearData()) */
	TSet<uint32>	KnownTrimmers;
};
#endif