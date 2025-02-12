#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AITask_FlyTo.h"
#include "Player/WeaponOwnerInterface.h"
#include "AbilitySystem/CustomAbilitySystemComponent.h"
#include "AbilitySystem/CustomGameplayAbility.h"
#include "GameFramework/Character.h"
#include "Gameplay/CustomSignificanceInterface.h"
#include "Kismet/GameplayStatics.h"
#include "AAIPawn.generated.h"

class UFloatingPawnMovement;
class UBrainComponent;

UCLASS(Blueprintable, BlueprintType)
class MAIN_API AAIPawn : public ACharacter, public FCustomSignificanceInterface, public IAbilitySystemInterface, public IWeaponOwnerInterface
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCustomAbilitySystemComponent* AbilitySystemComponent;

	virtual void OnNearWeaponHit(const FHitResult& HitResult, const FName& HitSocketName) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void AddAbilities();

	UFUNCTION(BlueprintImplementableEvent)
	void K2_AddAbilities();

	UFUNCTION(BlueprintCallable)
	FGameplayAbilitySpecHandle K2_GiveAbility(TSubclassOf<UCustomGameplayAbility> Ability, int32 Lv = 1);

	UFUNCTION(BlueprintCallable)
	float GetAttributeValueByName(FName AttributeName);

	UFUNCTION(BlueprintImplementableEvent)
	USkeletalMeshComponent* GetRightHandHoldWeaponMesh() const;

	UFUNCTION(BlueprintImplementableEvent)
	USkeletalMeshComponent* GetLeftHandHoldWeaponMesh() const;

	virtual USkeletalMeshComponent* GetRightWeaponMesh() const override;

	virtual USkeletalMeshComponent* GetLeftWeaponMesh() const override;

	UFUNCTION()
	void InitAttributesFromLua(TMap<FString, float> InAttributes);

	UPROPERTY(Transient)
	class UCombatAttributeSet* CombatAttributeSet;

	void OnDead(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec,
	            float DamageMagnitude, AActor* AvatarActor);

	UFUNCTION(BlueprintCallable)
	float GetHealth();

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth();

	UFUNCTION(BlueprintImplementableEvent)
	float Die(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude, AActor* AvatarActor);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTwoFloatEvent, float, PrevValue, float, AfterValue);

	UPROPERTY()
	FTwoFloatEvent OnHealthChange;
	UPROPERTY()
	FTwoFloatEvent OnMaxHealthChange;

	static FName AbilitySystemComponentName;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_IsDead)
	bool IsDead = false;

	UFUNCTION(BlueprintCallable)
	void OnRep_IsDead();

public:
	UFUNCTION()
	void SetUpProperties();

	UFUNCTION(BlueprintCallable, Category="AIPawn")
	void FindRandomLocation(const FVector& CenterLocation, FVector& ResultLocation, const float Range);

	UFUNCTION(BlueprintCallable, Category="AIPawn")
	virtual UAITask_MoveTo* MoveToLocation(FVector Position, float AcceptanceRadius = -1.f,
	                                       EAIOptionFlag::Type StopOnOverlap = EAIOptionFlag::Default,
	                                       EAIOptionFlag::Type AcceptPartialPath = EAIOptionFlag::Default,
	                                       bool bUsePathfinding = true, bool bLockAILogic = true,
	                                       bool bUseContinuosGoalTracking = false,
	                                       EAIOptionFlag::Type ProjectGoalOnNavigation = EAIOptionFlag::Disable);

	UFUNCTION(BlueprintCallable, Category="AIPawn")
	virtual UAITask_MoveTo* MoveToActor(AActor* ActorPtr, float AcceptanceRadius = -1.f,
	                                    EAIOptionFlag::Type StopOnOverlap = EAIOptionFlag::Default,
	                                    EAIOptionFlag::Type AcceptPartialPath = EAIOptionFlag::Default,
	                                    bool bUsePathfinding = true, bool bLockAILogic = true,
	                                    bool bUseContinuosGoalTracking = false,
	                                    EAIOptionFlag::Type ProjectGoalOnNavigation = EAIOptionFlag::Disable);

	UPROPERTY()
	UAITask_MoveTo* MoveToTaskPtr;

	UPROPERTY()
	UAITask_FlyTo* FlyToTaskPtr;

	void FlyTo(FVector InGoalLocation, AActor* InGoalActor,
	           float AcceptanceRadius, EAIOptionFlag::Type StopOnOverlap, EAIOptionFlag::Type AcceptPartialPath,
	           bool bUsePathfinding, bool bLockAILogic, bool bUseContinuousGoalTracking,
	           EAIOptionFlag::Type ProjectGoalOnNavigation);

	void MoveTo(FVector InGoalLocation, AActor* InGoalActor,
	            float AcceptanceRadius, EAIOptionFlag::Type StopOnOverlap, EAIOptionFlag::Type AcceptPartialPath,
	            bool bUsePathfinding, bool bLockAILogic, bool bUseContinuousGoalTracking,
	            EAIOptionFlag::Type ProjectGoalOnNavigation);

	UFUNCTION(BlueprintCallable, Category="AIPawn")
	void SetSpeed(float Speed);

	UFUNCTION(BlueprintCallable, Category="AIPawn")
	float GetSpeed();

	UFUNCTION(BlueprintCallable, Category="AIPawn")
	void StopMovementImmediately(const bool bClearDelegate = false);

public:
	UFUNCTION(BlueprintCallable, Category="AIPawn")
	void SetRotationFollowVelocity(bool bFollow);

	UFUNCTION(BlueprintCallable, Category="AIPawn")
	void LockLookAtPos(const FVector& TargetLocation, float UsedRotateSpeed = -1);

	UFUNCTION(BlueprintCallable, Category="AIPawn")
	void LockLookAtActor(AActor* Actor, float UsedRotateSpeed = -1);

	UFUNCTION(BlueprintCallable, Category="AIPawn")
	void UnlockLookAt();

	UFUNCTION(BlueprintCallable, Category="AIPawn")
	void RotateToTargetRotation(const FRotator& Rotation, float UsedRotateSpeed = -1);

	UPROPERTY(BlueprintReadWrite, Category="AIPawn")
	float RotateSpeed = 0;

	float CurrentRotateSpeed = 0;

	bool bRotationFollowVelocity = false;

	bool bLockRotationToActor;

	bool bLockRotationToLocation;

	TWeakObjectPtr<AActor> LockedActor;

	FVector LockedLocation;

	FRotator* TargetRotator = NULL;

public:
	void SetTickInterval(float Interval);

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void UpdateSignificanceSettings(const TMap<float, float>& SignificanceThresholdsMap);

	virtual float SignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, const FTransform& Viewpoint) override;
	virtual void PostSignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance, float Significance, bool bFinal) override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
