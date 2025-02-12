
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "CustomPlayerState.generated.h"

UCLASS()
class MAIN_API ACustomPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ACustomPlayerState();

	// Implement IAbilitySystemInterface
	virtual  UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UCombatAttributeSet* GetCombatAttributeSet() const;
	
	virtual void CopyProperties(APlayerState* PlayerState) override;

protected:
	UPROPERTY()
	class UCustomAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(Transient)
	UCombatAttributeSet* CombatAttributeSet;
};
