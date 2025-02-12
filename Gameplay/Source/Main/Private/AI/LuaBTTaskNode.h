#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "LuaBTTaskNode.generated.h"

UCLASS()
class MAIN_API ULuaBTTaskNode : public UBTTask_BlueprintBase, public IUnLuaInterface
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
    virtual FName GetNodeIconName() const override {return FName("BTEditor.Graph.BTNode.Task.Icon");};
#endif // WITH_EDITOR
};
