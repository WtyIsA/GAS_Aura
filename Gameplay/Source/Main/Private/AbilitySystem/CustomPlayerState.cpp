#include "AbilitySystem/CustomPlayerState.h"
#include "AbilitySystem/CustomAbilitySystemComponent.h"
#include "Attributes/CombatAttributeSet.h"

ACustomPlayerState::ACustomPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UCustomAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("CombatAttributeSet"));
	NetUpdateFrequency = 100.0f;

	AbilitySystemComponent->AddAttributeSetSubobject(CombatAttributeSet);
}

UAbilitySystemComponent * ACustomPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UCombatAttributeSet * ACustomPlayerState::GetCombatAttributeSet() const
{
	return CombatAttributeSet;
}

void ACustomPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	if(ACustomPlayerState* CustomPlayerState = Cast<ACustomPlayerState>(PlayerState))
	{
		CustomPlayerState->CombatAttributeSet = CombatAttributeSet;
		CustomPlayerState->AbilitySystemComponent = AbilitySystemComponent;
		CustomPlayerState->NetUpdateFrequency = 100.0f;
	}
}


