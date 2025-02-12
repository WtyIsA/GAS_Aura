#pragma once

#include "CoreMinimal.h"

#include "UObject/NoExportTypes.h"
#include "UnLuaInterface.h"
#include "Engine/StreamableManager.h"
#include "ResMgr.generated.h"
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnResLoadComplete, UObject*,obj, FString, resPath);
UCLASS()
class UResMgr : public UObject, public IUnLuaInterface
{
    GENERATED_BODY()
public:
    static UResMgr* Get();
    static void Free();

    virtual FString GetModuleName_Implementation() const  {
        return TEXT("Share.ResMgr");
    }

    UFUNCTION()
    UObject* SyncLoadRes(FString resPath);
    
    UFUNCTION()
    void AsyncLoadRes(FString resPath,const FOnResLoadComplete& callback);

private:
    FStreamableManager m_pResMgr;

    static  UResMgr* m_handle;
};
