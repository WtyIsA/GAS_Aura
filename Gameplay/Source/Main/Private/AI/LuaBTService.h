#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "LuaBTService.generated.h"

UCLASS()
class MAIN_API ULuaBTService : public UBTService_BlueprintBase, public IUnLuaInterface
{
    GENERATED_BODY()

    public:
	
    UPROPERTY(EditAnywhere, Category="Lua")
    FString LuaModule;
	
    virtual FString GetModuleName_Implementation() const override
    {
        return LuaModule;
    }

    virtual void OnInstanceDestroyed(UBehaviorTreeComponent& OwnerComp) override
    {
        Super::OnInstanceDestroyed(OwnerComp);
        MarkPendingKill();
    }
    

#if WITH_EDITOR
    virtual FName GetNodeIconName() const override {return FName("BTEditor.Graph.BTNode.Service.Icon");};
#endif // WITH_EDITOR
};
