// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "BaseGunWeapon.h"
#include "FSMSystem.h"
#include "GameFramework/Character.h"
#include "LuaManger.h"
#include "Gameplay/SyncMoveComponent.h"
#include "UI/Joystick/FTouchAxisInputPreprocessor.h"
#include "WeaponOwnerInterface.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/CustomAttributeSet.h"
#include "AbilitySystem/CustomAbilitySystemComponent.h"
#include "AbilitySystem/CustomPlayerState.h"
#include "AbilitySystem/CustomGameplayAbility.h"
#include "EnhancedInputComponent.h"
#include "Gameplay/CustomSignificanceInterface.h"
#include "MyShootCharacter.generated.h"


UENUM(BlueprintType)
enum class EGait:uint8
{
	Walking,
	Runing,
	Spring
};

UENUM(BlueprintType)
enum class EClimbType:uint8
{
	Cross,
	StandCross,
	HighDownClimb,
	HighClimb
};

UENUM(BlueprintType)
enum class EOverlayType:uint8
{
	Default,
	Rifle,
	Pistol,
	Knife,
	TakeItem
};


UCLASS(config=Game)
class AMyShootCharacter : public ACharacter, public FInputPreProcessInterface,public  FCustomSignificanceInterface,public IWeaponOwnerInterface,public IAbilitySystemInterface
{
public:
	UFUNCTION(BlueprintCallable)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFUNCTION(BlueprintCallable)
	FGameplayAbilitySpecHandle K2_GiveAbility(TSubclassOf<UCustomGameplayAbility> Ability, int32 InputID, int32 Lv = 1);
	UFUNCTION(BlueprintImplementableEvent)
	void K2_AddAbilities();
protected:
	UPROPERTY()
	UCustomAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
	UCombatAttributeSet* CombatAttributeSet;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	UFUNCTION(BlueprintCallable)
	virtual void AddAbilities();
	virtual void OnRep_PlayerState() override;
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void InitCharacter();
	UFUNCTION(BlueprintImplementableEvent)
	void AttributeSetInitFinish();

	UFUNCTION()
	void InitAttributesFromLua(TMap<FString,float> InAttributes);
	
	void OnGEAppliedToSelf(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
	
	UFUNCTION(Client, Reliable)
	void OnGetHurt(AActor* FromCharacter, int DamageAttributeType, int FromPlayerOrSystem);
	void OnGetHurt_Implementation(AActor* FromCharacter, int DamageAttributeType, int FromPlayerOrSystem);
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnGetHurt(AActor* FromCharacter, int DamageAttributeType, int FromPlayerOrSystem);
	UFUNCTION()
	void AbilityLocalInputPressed(int32 AbilityEnum);
	UFUNCTION()
	void AbilityLocalInputReleased(int32 AbilityEnum);
	UFUNCTION()
    void LocalInputConfirm();
    UFUNCTION()
    void LocalInputCancel();
public:
	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION()
	void Cheat_Invincible(bool bOpen);
	UFUNCTION(BlueprintNativeEvent)
	void OnReceiveAttributeChange();
	
private:
	GENERATED_BODY()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNearWeaponHit, FHitResult, hitInfo, FName,HitSocketName);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UFUNCTION(BlueprintCallable)
	void BindLuaFunction(const UInputAction* Action, ETriggerEvent TriggerEvent, FEnhancedInputActionHandlerDynamicSignature Callback);
	UFUNCTION()
	void LuaAddMappingContext(const UInputMappingContext* MappingContext, int32 Priority);
	UFUNCTION()
	void LuaRemoveMappingContext(const UInputMappingContext* MappingContext);
public:
	AMyShootCharacter();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UFSMSystem* FSM;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Input, Replicated)
	FVector2D MoveAxis;

	UPROPERTY(BlueprintReadWrite, Category=Input)
	bool bAim = false;
	UPROPERTY(BlueprintReadOnly, Category=Input)
	bool bAds = false;

	UPROPERTY(BlueprintReadOnly, Category=Move)
	FRotator SmallEyeRotation ;

	UPROPERTY(BlueprintReadOnly, Category=Move)
	bool EnableMove = false;
	UPROPERTY(BlueprintReadOnly, Category=Move)
	float MoveState = 0;

	UPROPERTY(BlueprintReadOnly, Category=Move)
	float Angle = 0;

	UPROPERTY(BlueprintReadWrite, Category=Weapon)
	bool bWantChangeWeapon = false;

	UPROPERTY(BlueprintReadWrite, Category=Move)
	float Yaw = 0;
	UPROPERTY(BlueprintReadWrite, Category=Move)
	float Pitch = 0;

	UPROPERTY(BlueprintReadOnly, Category=Move)
	float StopAngle = 0;

	UPROPERTY(BlueprintReadWrite, Category=Move)
	bool bDownSmallEye = false;

	UPROPERTY(BlueprintReadOnly, Category=Move)
	float InputAxisYaw = 0;

	UPROPERTY(BlueprintReadOnly, Category=Move, Replicated)
	FRotator PlayerControlRotation = FRotator::ZeroRotator;
	FRotator LastSendPlayerControlRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly, Category=Move)
	float InputAxisPitch = 0;

	UPROPERTY(BlueprintReadOnly)
	FTransform LeftHandTransform;

	UPROPERTY(BlueprintReadOnly)
	FTransform RightHandTransform;

	UPROPERTY(BlueprintReadOnly)
	FTransform AimRelativeTohand;


	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	ABaseGunWeapon* WeaponLeft;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	ABaseGunWeapon* WeaponRight;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	ABaseGunWeapon* WeaponHand;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	const USkeletalMeshSocket* WeaponPosLeft;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	const USkeletalMeshSocket* WeaponPosRight;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	const USkeletalMeshSocket* WeaponPosLeg;

	UPROPERTY(BlueprintReadWrite, Category=state)
	bool bAttacking;


	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* FpsArms;

	UPROPERTY(BlueprintReadWrite)
	bool bFpsCamera = false;
	
	UPROPERTY(BlueprintReadWrite)
	FVector Acceleration;

	UPROPERTY(BlueprintReadWrite)
	FVector PreviousVelocity;

	UPROPERTY(BlueprintReadWrite)
	bool HasMovementInput;
	float MovementInputAmount;

	USpringArmComponent* SpringArmComponent;
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAnimInstance* MainAnimInstance;
	UAnimInstance* FPSArmsAnimInstance;

	EGait TargetGait = EGait::Runing;
	EGait AllowedGait = EGait::Runing;

	UPROPERTY(BlueprintReadOnly)
	FRotator ControlRotationActorRotationDif;

	UPROPERTY(BlueprintReadWrite, Category=Gun)
	FVector GunFireRecedeTarget;
	
	int PlayerIndex = 0;

	UPROPERTY(BlueprintReadWrite)
	EOverlayType OverlayType;

	UFUNCTION(BlueprintCallable)
	void SetOverlayType(EOverlayType type)
	{
		OverlayType = type;
	}
	void PlayMontage(const TCHAR*);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	UFUNCTION(BlueprintCallable)
	void MoveForwardCpp(float Value);
	UFUNCTION(BlueprintCallable)
	void MoveRightCpp(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void UpdateEnableMove();
	void UpdateMoveState();

	UFUNCTION(BlueprintCallable)
	void UpdateDirection();

	UFUNCTION(BlueprintCallable)
	void SetUROParam(USkeletalMeshComponent* m);

	UFUNCTION(BlueprintCallable)
	FVector GetLocalVelocity();

	UFUNCTION(BlueprintCallable)
	void UpdatePitchYaw();

	void UpdateHasMovementInput();

	UFUNCTION(BlueprintCallable)
	void ReSetWorldOrginPos();
	void UpdateControlRotationActorRotationDif();

	UFUNCTION(Server, Unreliable)
	void ServerMoveAxis(FVector2D v);

	UFUNCTION(Server, Unreliable)
	void ServerSetControlRotation(FRotator rotator);
	float lastSendTime = 0;

	UFUNCTION(BlueprintImplementableEvent)
	void CancelAim();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION(BlueprintImplementableEvent)
	void LuaSetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

	void OnRootComponentTransformUpdated(USceneComponent* /*UpdatedComponent*/, EUpdateTransformFlags /*UpdateTransformFlags*/, ETeleportType );

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(BlueprintReadWrite)
	float DefaultFov = 90;

	UPROPERTY(BlueprintReadOnly)
	float TargetFov;

	UFUNCTION(BlueprintCallable)
	void MyAddControllerYawInput(float val);

	UFUNCTION(BlueprintCallable)
	void MyAddControllerPitchInput(float val);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHandAttack();
	
	template <class T>
	T* LoadActorByBlueprint(UClass*, const TCHAR* path);

	void Print(FString s);

	UFUNCTION(BlueprintCallable)
	int32 AddPlayerMappedKey(const FName MappingName, const struct FKey NewKey, bool bForceImmediately = false);
	UFUNCTION(BlueprintCallable)
	int32 RemovePlayerMappedKey(const FName MappingName, const bool bForceImmediately = false);
	UFUNCTION(BlueprintCallable)
	FKey GetPlayerMappedKey(const FName MappingName) const;
	UFUNCTION(BlueprintCallable)
	void RemoveAllPlayerMappedKeys(bool bForceImmediately = false);
	
	UPROPERTY(BlueprintAssignable)
	FOnNearWeaponHit OnNearWeaponHitEvent;

	UFUNCTION(BlueprintNativeEvent)
	void OnActorLocationChange(FVector Location);

	UFUNCTION(BlueprintNativeEvent)
    void OnActorRotationChange(FRotator Rotator);

	UFUNCTION(BlueprintImplementableEvent)
	bool IsRightHandAttack();
	
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool AddYawInput(float value);
	virtual bool AddPitchInput(float value);

	UFUNCTION(BlueprintCallable)
	void OpenSignificanceSystem();
	UFUNCTION(BlueprintCallable)
	void UpdateSignificanceSettings(const TMap<float,float>& SignificanceThresholdsMap);
	virtual float SignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo,
									   const FTransform& Viewpoint) override;
	virtual void PostSignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance,
										  float Significance, bool bFinal) override;

	UPROPERTY()
	float Min_Moved_Square_Distance = 10000;	

	UPROPERTY()
	float Min_Delta_Roll = 361;

	UPROPERTY()
	float Min_Delta_Pitch = 361;

	UPROPERTY()
	float Min_Delta_Yaw = 10;

	bool IsRotationChanged(FRotator) const;
	void SetTickInterval(float interval);
	UFUNCTION(BlueprintCallable)
	void SetAllTickIntervals(float interval);
	UFUNCTION(BlueprintImplementableEvent)
    USkeletalMeshComponent* GetRightHandHoldWeaponMesh() const;
	UFUNCTION(BlueprintImplementableEvent)
    USkeletalMeshComponent* GetLeftHandHoldWeaponMesh() const;
	virtual USkeletalMeshComponent* GetRightWeaponMesh() const override;
	virtual USkeletalMeshComponent* GetLeftWeaponMesh() const override;
	virtual void OnNearWeaponHit(const FHitResult& HitResult, const FName& HitSocketName) const override;

	UFUNCTION(BlueprintCallable)
	float GetHealth();
	
	UFUNCTION(BlueprintCallable)
	float GetMaxHealth();

	UFUNCTION(BlueprintCallable)
	float GetAttributeValueByName(FName AttributeName);
	
	UFUNCTION(BlueprintImplementableEvent)
	void LuaOnDie(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec& EffectSpec, float EffectMagnitude,AActor* AvatarActor);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTwoFloatEvent, float, PrevValue, float, AfterValue);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOneStringTwoFloatEvent, FString, AttributeName, float, PrevValue, float, AfterValue);

	UPROPERTY()
	FTwoFloatEvent OnHealthChange;
	UPROPERTY()
	FTwoFloatEvent OnMaxHealthChange;
	UPROPERTY()
	FTwoFloatEvent OnManaChange;
	UPROPERTY()
	FTwoFloatEvent OnMaxManaChange;
	UPROPERTY()
	FOneStringTwoFloatEvent OnAttributeChange;

	UFUNCTION()
	void DoBindInputComponent();

private:
	void OnAttributeChangeCallback(FGameplayAttribute Attribute, float PrevValue, float AfterValue);

	void OnDie(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec& EffectSpec, float EffectMagnitude,AActor* AvatarActor);
	
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttributeSetInitFinish);
	
    UPROPERTY()
	FOnAttributeSetInitFinish OnAttributeSetInitFinish;
	
private:
	TSharedPtr<class FTouchAxisInputPreprocessor> AxisInputPreprocessor;
	bool m_bRegisterInputPreProcess;
	bool bOpenedSignificance =false;
	USyncMoveComponent* SyncMoveComponent;
	FVector LastLocation;
	FRotator LastRotator;
	bool bASCBindInput = false;
};



inline void AMyShootCharacter::Print(FString s)
{
	UE_LOG(GAME, Display, TEXT("[Player]  %s"), *s);
}
