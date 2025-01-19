#include "ResMgr.h"

#include <functional>

#include "LuaManger.h"
#include "Aura/AuraLogChannels.h"

UResMgr* UResMgr::m_handle = NULL;

UResMgr* UResMgr::Get()
{
	if (m_handle == nullptr)
	{
		m_handle = NewObject<UResMgr>(ULuaManger::m_pGame);
		m_handle->AddToRoot();
	}
	return m_handle;
}

void UResMgr::Free()
{
	if (m_handle)
	{
		m_handle->RemoveFromRoot();
		m_handle = nullptr;
	}
}

UObject* UResMgr::SyncLoadRes(FString resPath)
{
	if (resPath.Len() == 0)
		return NULL;
	TSharedPtr<FStreamableHandle> handle = m_pResMgr.RequestSyncLoad(resPath);
	if (handle.IsValid())
		return handle.Get()->GetLoadedAsset();
	return nullptr;
}

void UResMgr::AsyncLoadRes(FString resPath, const FOnResLoadComplete& callback)
{
	if (resPath.Len() == 0)
	{
		callback.ExecuteIfBound(nullptr, resPath);
		return;
	}

	TArray<FStringAssetReference> ObjToLoad;
	ObjToLoad.Add(resPath);
	TSharedPtr<FStreamableHandle> reshandle = m_pResMgr.RequestAsyncLoad(ObjToLoad, [resPath, callback]()
	{
		UObject* pObject = StaticFindObject(UObject::StaticClass(), nullptr, *resPath);
		if (pObject == nullptr)
		{
			UE_LOG(LogAura, Warning, TEXT("UResMgr::AsyncLoadResload res %s error"), *resPath);
			return;
		}
		callback.ExecuteIfBound(pObject, resPath);
	});
}
