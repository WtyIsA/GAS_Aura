// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AuraCharacterBase.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;

// Abstract 抽象类不能被拖到Level中，可以防止基类被拖放
UCLASS(Abstract)
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Combit")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void InitAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;
	
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameEffectClass, float Level) const;

	void InitializeDefaultAttribute() const;
};
