#include "MemoryTracker.h"

#include "LuaManger.h"
#if ENABLE_MEMORYTRACKER
const TCHAR* FMemoryStatsCallstack::CallstackSeparator = TEXT( "+" );
FMemoryTracker* FMemoryTracker::m_Ins = nullptr;
FMemoryTracker::FMemoryTracker()
{	
	BShouldTracker = false;
	NumIngore = 6;
	NumStackList = 10;
	bRecursive = false;
}

FMemoryTracker* FMemoryTracker::Get()
{
	if(m_Ins == nullptr)
		m_Ins = new FMemoryTracker();
	return m_Ins;
}

void FMemoryTracker::Free()
{
	if(m_Ins)
		delete m_Ins;
	m_Ins = nullptr;
}

void FMemoryTracker::Init()
{
	if(!IsRunningDedicatedServer())
		return;
	//AddTag(24);
	if(FLowLevelMemTracker::Get().bIsDisabled)
		return;
#if ENABLE_MEMORYTRACKER
	FLowLevelMemTracker::Get().bIsDisabled = true;
	FLowLevelMemTracker::Get().OnMemeryAlloc.AddLambda([this](const void* ptr,  int64 size, uint64 tag)
	{
		this->OnAlloc(ptr, size, tag);
	});
	FLowLevelMemTracker::Get().OnMemeryFree.AddLambda([this](const void* ptr,  int64 size, uint64 tag)
	{
		this->OnFree(ptr, size, tag);
	});
	FLowLevelMemTracker::Get().bIsDisabled = false;
#endif	
}

void FMemoryTracker::OnAlloc(const void* ptr, int64 size, uint64 tag)
{
	if(!BShouldTracker)
		return;
	FScopeLock Lock(&MemCritical);
	if(bRecursive)
		return;
	if(!TrackerTags.Contains(tag))
		return;
	bRecursive = true;
	FCallstackTrack Callstack;
	FPlatformStackWalk::CaptureStackBackTrace(Callstack.CallStack, FCallstackTrack::Depth);
	Callstack.Size = size;
	Callstack.Tag = tag;
	AddCallstack(Callstack);
	OpenPointers.Add(ptr, Callstack);
	bRecursive = false;
}

void FMemoryTracker::OnFree(const void* Ptr, int64 size, uint64 tag)
{
	FScopeLock Lock(&MemCritical);
	if(bRecursive)
		return;
	bRecursive= true;
	FCallstackTrack* Callstack = OpenPointers.Find(Ptr);
	if (Callstack)
	{
		if(size != Callstack->Size)
		{
			UE_LOG(LogTemp,Display,TEXT("FMemoryTracker onFree %lld != %lld"), size, Callstack->Size);
		}
		RemoveCallstack(*Callstack);
		KnownDeleters.Add(Callstack->GetHash());
		OpenPointers.Remove(Ptr);
	}
	bRecursive = false;
}

bool FMemoryTracker::SetTracker(bool bTracker, int nIngore, int nStackNum)
{
	BShouldTracker = bTracker;	
	if(nIngore > 0)
		NumIngore = nIngore;
	if(nStackNum > 0)
		NumStackList = nStackNum;
	return true;
}

void FMemoryTracker::SaveTag(const FString& strTag, const FString& strSavePath)
{
	uint64 nTag = 0;
	if(!FLowLevelMemTracker::Get().FindTagByName(*strTag, nTag))
	{
		UE_LOG(GAME,Display,TEXT("[mem]FindTagByName error %s"),*strTag);
		return;
	}
	SaveTag(nTag, strSavePath);
}


void FMemoryTracker::SaveTag(uint64 tag, const FString& strsavePath)
{
	FScopeLock Lock(&MemCritical);
	if(bRecursive)
		return;
	uint64 nTotalMemoryBeforeSave = FLowLevelMemTracker::Get().GetTotalTrackedMemory(ELLMTracker::Platform);	
	bRecursive = true;
	UE_LOG(GAME,Display,TEXT("tag %d is %s"), tag, *FLowLevelMemTracker::Get().FindTagDisplayName(tag).ToString())
	FString strLog;
	FString strPath=  FPaths::GetPath(strsavePath);
	ULuaManger::Get()->CreateDir(strPath);
	int nFileId = 0;
	strPath = strsavePath;
	int64 time = (int64)FPlatformTime::Seconds();
	while (FPaths::FileExists(strPath))
	{
		nFileId++;
		strPath = FString::Printf(TEXT("%s_%d.csv"), *strsavePath, nFileId);
	}
	TArray<FName>stackList;
	int nCount = 0;
	int64 nTotalSize = 0;
	bool bSaveMutlFiles = false;
	int nMutlFileId = 0;
	if(MemoryInfo.Num() > 20000)
	{
		bSaveMutlFiles = true;
		strPath = FString::Printf(TEXT("%s_%d_%d.csv"), *strsavePath, nFileId, nMutlFileId);
	}
	const int32 MaxCallstackLineChars = 2048;
	char CallstackString[MaxCallstackLineChars];
	TCHAR CallstackStringWide[MaxCallstackLineChars];
	FMemory::Memzero(CallstackString);
	for(auto& item: UniqueCallstacks)
	{
		bool KnownDeleter = KnownDeleters.Contains(item.Key);
		//if(KnownDeleter)
		//	continue;
		const FCallstackTrack& Callstack = item.Value;			

		nCount++;
		bool KnownTrimmer = KnownTrimmers.Contains(Callstack.CachedHash);		

		FString strStack;
		for (int32 i = NumIngore; i < FCallstackTrack::Depth; ++i)
		{
			FPlatformStackWalk::ProgramCounterToHumanReadableString(i, Callstack.CallStack[i], CallstackString, MaxCallstackLineChars);

			if (Callstack.CallStack[i] && FCStringAnsi::Strlen(CallstackString) > 0)
			{
				//convert ansi -> tchar without mallocs in case we are in OOM handler.
				for (int32 CurrChar = 0; CurrChar < MaxCallstackLineChars; ++CurrChar)
				{
					CallstackStringWide[CurrChar] = CallstackString[CurrChar];
				}

				strStack.Appendf(TEXT("%s\r\n"), CallstackStringWide);
			}
			FMemory::Memzero(CallstackString);
		}

		strLog.Appendf(TEXT("%p,%lld,%lld,%d,\"%s\"\r\n"),item.Key,Callstack.Tag, Callstack.Size,  Callstack.Count, *strStack);
		if(bSaveMutlFiles && nCount == 10000)
		{
			ULuaManger::Get()->SaveStringToFile(strPath, strLog);
			strLog.Empty();
			nMutlFileId++;
			nCount = 0;
			strPath = FString::Printf(TEXT("%s_%d_%d.csv"), *strsavePath, nFileId, nMutlFileId);
		}
	}
	for (auto& item : MemoryInfo)
	{
		FMemoryInfo* pInfo = &item.Value;
		if(pInfo->Tag == tag)
		{
			nCount++;
			nTotalSize += pInfo->Size;
			FMemoryStatsCallstack::DecodeToNames(*pInfo->EncodedCallstack, stackList);
			FString strStack;
			for(int i = 0; i < stackList.Num(); i++)
			{
				strStack.Appendf(TEXT("%s\r\n"),*stackList[i].ToString());
			}
			stackList.Empty();
			strLog.Appendf(TEXT("%p,%lld,%lld,%lld,\"%s\"\r\n"),item.Key,pInfo->Tag, pInfo->Size,  time - pInfo->Time, *strStack);
			if(bSaveMutlFiles && nCount == 10000)
			{
				ULuaManger::Get()->SaveStringToFile(strPath, strLog);
				strLog.Empty();
				nMutlFileId++;
				nCount = 0;
				strPath = FString::Printf(TEXT("%s_%d_%d.csv"), *strsavePath, nFileId, nMutlFileId);
			}
		}
	}
	
	strPath = FString::Printf(TEXT("%s_%d.csv"), *strsavePath, nFileId);
	ULuaManger::Get()->SaveStringToFile(strPath, strLog);
	strLog.Empty();
	
	//strLog.Appendf(TEXT("totalsize=%lld, count=%d"), nTotalSize, nCount);
	uint64 nTotalMemoryAfterSave = FLowLevelMemTracker::Get().GetTotalTrackedMemory(ELLMTracker::Platform);
	UE_LOG(LogTemp,Display,TEXT("[memory]check memory leak, totalmemory change from %lld to %lld,  inc %lld"), nTotalMemoryBeforeSave
		, nTotalMemoryAfterSave, nTotalMemoryAfterSave - nTotalMemoryBeforeSave);
	bRecursive = false;
}


void FMemoryTracker::AddTag(uint64 nTag)
{
	FScopeLock Lock(&MemCritical);
	if(bRecursive)
		return;
	bRecursive = true;
	if(!TrackerTags.Contains(nTag))
		TrackerTags.Add(nTag);
	bRecursive = false;
}

void FMemoryTracker::AddTag(const FString& strTag)
{
	uint64 nTag = 0;
	if(!FLowLevelMemTracker::Get().FindTagByName(*strTag, nTag))
	{
		UE_LOG(GAME,Display,TEXT("[mem]FindTagByName error %s"),*strTag);
		return;
	}
	AddTag(nTag);
	
}


void FMemoryTracker::RemoveTag(uint64 nTag)
{
	FScopeLock Lock(&MemCritical);
	if(bRecursive)
		return;
	bRecursive = true;
	TrackerTags.Remove(nTag);
	bRecursive = false;
}

void FMemoryTracker::RemoveTag(const FString& strTag)
{
	uint64 nTag = 0;
	if(!FLowLevelMemTracker::Get().FindTagByName(*strTag, nTag))
	{
		UE_LOG(GAME,Display,TEXT("[mem]FindTagByName error %s"),*strTag);
		return;
	}
	RemoveTag(nTag);	
}

void FMemoryTracker::Clear()
{
	MemoryInfo.Empty();
}

void FMemoryTracker::AddCallstack(FCallstackTrack& Callstack)
{
	uint32 CallstackHash = Callstack.GetHash();
	FCallstackTrack& UniqueCallstack = UniqueCallstacks.FindOrAdd(CallstackHash);
	//if we had a hash collision bail and lose the data rather than corrupting existing data.
	if (UniqueCallstack.Count > 0  && UniqueCallstack != Callstack)
	{
		ensureMsgf(false, TEXT("Callstack hash collision.  Throwing away new stack."));
		return;
	}

	if (UniqueCallstack.Count == 0)
	{
		UniqueCallstack = Callstack;
		UniqueCallstack.CachedHash = CallstackHash;
	}
	else
	{
		UniqueCallstack.Size += Callstack.Size;
	}
	UniqueCallstack.Count++;	
}

void FMemoryTracker::RemoveCallstack(FCallstackTrack& Callstack)
{
	uint32 CallstackHash = Callstack.GetHash();
	FCallstackTrack* UniqueCallstack = UniqueCallstacks.Find(CallstackHash);
	if (UniqueCallstack)
	{
		UniqueCallstack->Count--;
		UniqueCallstack->Size -= Callstack.Size;

		if (UniqueCallstack->Count == 0)
		{
			UniqueCallstack = nullptr;
			UniqueCallstacks.Remove(CallstackHash);
		}		
	}
}
#endif