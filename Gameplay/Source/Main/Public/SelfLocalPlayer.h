#pragma once

#include "Engine/LocalPlayer.h"
#include "CoreMinimal.h"
#include "SelfLocalPlayer.generated.h"

UCLASS()
class USelfLocalPlayer : public ULocalPlayer
{
    GENERATED_BODY()
    
public:
    USelfLocalPlayer();

    UFUNCTION()
    void AddOption(FString Key,FString Value);

    UFUNCTION()
    void RemoveOption(FString Key);

    UFUNCTION()
    void ClearAllOptions(); 

    virtual FString GetGameLoginOptions() const override;

private:  
    void InitializeDefaults();
    TMap<FString, FString> OptionalData;
};
