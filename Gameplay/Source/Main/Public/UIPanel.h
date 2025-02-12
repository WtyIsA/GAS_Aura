#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "UnLuaInterface.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "UIPanel.generated.h"

UCLASS()
class MAIN_API UUIPanel : public UCommonActivatableWidget, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	static int HandleInstanceId;
	
	UPROPERTY(EditDefaultsOnly, Category="Lua")
	FString LuaModule;
	
	virtual FString GetModuleName_Implementation() const override
	{
		return LuaModule;
	}

	UFUNCTION()
	void ResetZOrderInViewPort(int32 zOrder);

	UUIPanel(const FObjectInitializer& ObjectInitializer);

protected:
#pragma region CommonUI
	UFUNCTION()
	int LuaRegisterUIActionBinding_Internal(const FDataTableRowHandle& InHandle, bool bDisplayInActionBar, const int InIndex);
	UFUNCTION()
	void LuaRemoveActionBinding(int InIndex);
	UFUNCTION()
	void LuaAddActionBinding(int InIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void CallLuaCommonUIFunction(int InIndex);
#pragma endregion 

#pragma region EnhancedInput
	UFUNCTION()
	void LuaAddMappingContext(APlayerController* PC, APawn* Pawn, const UInputMappingContext* MappingContext, int32 InPriority);
	UFUNCTION()
	void LuaRemoveMappingContext(const APlayerController* PC, const UInputMappingContext* MappingContext);
	UFUNCTION()
	void LuaBindInputAction_Internal(const APlayerController* PC, const UInputAction* Action, ETriggerEvent TriggerEvent, FEnhancedInputActionHandlerDynamicSignature Callback);
	UFUNCTION()
	void LuaResetInputActions(const APlayerController* PC);
#pragma endregion 

private:
	TMap<int,FUIActionBindingHandle> IndexToHandleMap;
};
