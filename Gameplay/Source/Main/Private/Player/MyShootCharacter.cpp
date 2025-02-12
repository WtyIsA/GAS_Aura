
#include "Player/MyShootCharacter.h"
#include <vector>

#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/CustomAbilitySystemComponent.h"
#include "AbilitySystem/CustomPlayerState.h"
#include "AbilitySystem/Attributes/CombatAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/InputDelegateBinding.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Gameplay/SyncMoveComponent.h"
#include "Input/ShortcutkeyMgr.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/Rotator.h"

AMyShootCharacter::AMyShootCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = false; 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;


	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true; 

	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	m_bRegisterInputPreProcess = false;
}

void AMyShootCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsRunningDedicatedServer())
	{
		return;
	}
	if (MainAnimInstance==nullptr)
	{
		return;
	}
	UpdateEnableMove();
	UpdateMoveState();
	UpdateDirection();
	UpdatePitchYaw();
	UpdateHasMovementInput();
	UpdateControlRotationActorRotationDif();
}


void AMyShootCharacter::UpdateControlRotationActorRotationDif()
{
	ControlRotationActorRotationDif = UKismetMathLibrary::NormalizedDeltaRotator(
		PlayerControlRotation, GetActorRotation());
}


void AMyShootCharacter::ServerMoveAxis_Implementation(FVector2D v)
{
	MoveAxis= v;
}

void AMyShootCharacter::MyAddControllerYawInput(float val)
{
	
	if (IsLocallyControlled())
	{
		InputAxisYaw = val;
		AddControllerYawInput(val);
		if (!bDownSmallEye)
		{
			PlayerControlRotation= GetControlRotation();
		}
		//ServerSetControlRotation(PlayerControlRotation);
	}
	
}

void AMyShootCharacter::MyAddControllerPitchInput(float val)
{

	if (IsLocallyControlled())
	{
		InputAxisPitch = val;
		AddControllerPitchInput(val);
		if (!bDownSmallEye)
		{
			PlayerControlRotation= GetControlRotation();
		}
		if (UKismetMathLibrary::Abs(LastSendPlayerControlRotation.Pitch -PlayerControlRotation.Pitch)>2)
		{
			ServerSetControlRotation(PlayerControlRotation);
			LastSendPlayerControlRotation = PlayerControlRotation;
		}
	}
}


void AMyShootCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	UShortcutkeyMgr::Get()->AddExistKeyBinds();
	//InputComponent->BindAxis("Turn", this, &AMyShootCharacter::MyAddControllerYawInput);
	//InputComponent->BindAxis("LookUp", this, &AMyShootCharacter::MyAddControllerPitchInput);
	if ( !bASCBindInput && AbilitySystemComponent)
	{
		AbilitySystemComponent->BindToInputComponent(PlayerInputComponent);
		bASCBindInput = true;
	}
	LuaSetupPlayerInputComponent(PlayerInputComponent);
}

void AMyShootCharacter::OnRootComponentTransformUpdated(USceneComponent*, EUpdateTransformFlags, ETeleportType)
{
	const FVector Location = GetActorLocation();
	if(FVector::DistSquared(Location,LastLocation) >= Min_Moved_Square_Distance)
	{
		LastLocation = Location;
		OnActorLocationChange(Location);
	}
	const FRotator CurrentRotor = GetActorRotation();
	if(IsRotationChanged(CurrentRotor))
	{
		LastRotator = CurrentRotor;
		OnActorRotationChange(CurrentRotor);
	}
	FCoreDelegates::OnObjectNetPropertyChanged.Broadcast(this);
}

void AMyShootCharacter::BeginPlay()
{
	Super::BeginPlay();
	MainAnimInstance = GetMesh()->GetAnimInstance();
	SpringArmComponent = Cast<USpringArmComponent>(GetComponentByClass(USpringArmComponent::StaticClass()));
	CameraComponent = Cast<UCameraComponent>(GetComponentByClass(UCameraComponent::StaticClass()));
	FSM=Cast<UFSMSystem>(GetComponentByClass(UFSMSystem::StaticClass()));
	SyncMoveComponent = Cast<USyncMoveComponent>(GetComponentByClass(USyncMoveComponent::StaticClass()));
	bFpsCamera = false;
	
	if (IsLocallyControlled())
	{
		AxisInputPreprocessor = MakeShared<FTouchAxisInputPreprocessor>(this);
		FSlateApplication::Get().RegisterInputPreProcessor(AxisInputPreprocessor, 0);
		m_bRegisterInputPreProcess = true;
	}
	LastRotator = GetActorRotation();
	if(IsRunningDedicatedServer())
	{
		RootComponent->TransformUpdated.AddUObject(this, &AMyShootCharacter::OnRootComponentTransformUpdated);
		LastLocation = GetActorLocation();
		LastRotator = GetActorRotation();
	}
}

void AMyShootCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	SignificanceUnregisterObject(this,GetWorld());
	if (m_bRegisterInputPreProcess)
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(AxisInputPreprocessor);
		AxisInputPreprocessor.Reset();
		m_bRegisterInputPreProcess = false;
	}
}

void AMyShootCharacter::PlayMontage(const TCHAR* path)
{
	UAnimMontage* montage = LoadObject<UAnimMontage>(
		nullptr, path);
	if (montage != nullptr && montage->IsValidLowLevel())
	{
		MainAnimInstance->Montage_Play(montage);
	}
	else
	{
		Print("AMyShootCharacter::PlayMontage: Load Fail");
	}
}

void AMyShootCharacter::UpdateEnableMove()
{
	if (MoveAxis != FVector2D::ZeroVector)
	{
		EnableMove = true;
		StopAngle = Angle;
	}
	else
	{
		EnableMove = false;
	}
}

void AMyShootCharacter::UpdateMoveState()
{
	MoveState = (GetVelocity().Size() / GetCharacterMovement()->MaxWalkSpeed) * 2;
}


void AMyShootCharacter::UpdateDirection()
{
	const FRotator Rotation = PlayerControlRotation;
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * MoveAxis.X;
	const FVector right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * MoveAxis.Y;
	const FVector velocity = forward + right;

	Angle = MainAnimInstance->CalculateDirection(velocity, GetActorRotation());
}

void AMyShootCharacter::SetUROParam(USkeletalMeshComponent* m)
{
	if (m)
	{
		m->AnimUpdateRateParams->MaxEvalRateForInterpolation = 5;
		m->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Reset();
		m->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Emplace(0.01);
		m->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Emplace(0.003);
		m->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Emplace(0.0005);
		m->AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds.Emplace(0.0002);
		m->AnimUpdateRateParams->BaseNonRenderedUpdateRate =15;
	}
}

FVector AMyShootCharacter::GetLocalVelocity()
{
	FVector V = GetVelocity();
	FRotator R = GetActorRotation();
	V = R.UnrotateVector(V);
	return V;
}


void AMyShootCharacter::UpdatePitchYaw()
{
	const FRotator Rotator =PlayerControlRotation;
	if (Rotator.Pitch > 180)
	{
		Pitch = Rotator.Pitch - 360;
	}
	else
	{
		Pitch = Rotator.Pitch;
	}
	if (Rotator.Yaw > 180)
	{
		Yaw = Rotator.Yaw - 360;
	}
	else
	{
		Yaw = Rotator.Yaw;
	}
}

void AMyShootCharacter::UpdateHasMovementInput()
{
	MovementInputAmount = GetCharacterMovement()->GetCurrentAcceleration().Size() / GetCharacterMovement()->
		GetMaxAcceleration();
	if (MovementInputAmount > 0)
	{
		HasMovementInput = true;
	}
	else
	{
		HasMovementInput = false;
	}
}

void AMyShootCharacter::ReSetWorldOrginPos()
{
	if (IsLocallyControlled())
	{
		FVector selfPos= GetActorLocation();
		FIntVector origin= GetWorld()->OriginLocation;
		if (FMath::Abs( selfPos.X)>50000||FMath::Abs( selfPos.Y)>50000||FMath::Abs( selfPos.Z)>50000)
		{
			GetWorld()->SetNewWorldOrigin(FIntVector(selfPos.X,selfPos.Y,selfPos.Z)+origin);
		}
	}
}


void AMyShootCharacter::ServerSetControlRotation_Implementation(FRotator rotator)
{
	PlayerControlRotation= rotator;
}

void AMyShootCharacter::OnActorLocationChange_Implementation(FVector Location)
{
}

void AMyShootCharacter::OnActorRotationChange_Implementation(FRotator Rotator)
{
}

void AMyShootCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AMyShootCharacter,MoveAxis,COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AMyShootCharacter,PlayerControlRotation,COND_SkipOwner);
}

template <class T>
T* AMyShootCharacter::LoadActorByBlueprint(UClass* u, const TCHAR* path)
{
	UClass* blueprints = StaticLoadClass(u, nullptr, path);
	if (blueprints != nullptr)
	{
		T* actor = GetWorld()->SpawnActor<T>(blueprints);
		if (actor != nullptr)
		{
			return actor;
		}
		return nullptr;
	}
	return nullptr;
}


void AMyShootCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	//AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	UpdatePitchYaw();
}

void AMyShootCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	//AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	UpdatePitchYaw();
}

void AMyShootCharacter::MoveForwardCpp(float Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}
	MoveAxis.X = Value;
	//ServerMoveAxis(MoveAxis);
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = bDownSmallEye?SmallEyeRotation:Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyShootCharacter::MoveRightCpp(float Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}
	MoveAxis.Y = Value;
	//ServerMoveAxis(MoveAxis);
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = bDownSmallEye?SmallEyeRotation:Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

bool AMyShootCharacter::AddYawInput(float value)
{
	if(value == 0)
		return false;
	if (!IsLocallyControlled())
	{
		return false;
	}
	MyAddControllerYawInput(value);
	return true;
};
bool AMyShootCharacter::AddPitchInput(float value)
{
	if(value == 0)
		return false;
	if (!IsLocallyControlled())
	{
		return false;
	}
	MyAddControllerPitchInput(value);
	return true;
}

void AMyShootCharacter::OpenSignificanceSystem()
{
	if (!bOpenedSignificance)
	{
		return;
	}
	bOpenedSignificance =true;
	const TArray<FSignificanceSettings::FSignificanceThresholds> Thresholds{{4,3000},{3,7000},{2,11000},{1,15000},{0,25000}};
	SetSignificanceSettings(Thresholds);
	//SignificanceRegisterObject(this,TEXT("Character"),GetWorld());
}

void AMyShootCharacter::UpdateSignificanceSettings(const TMap<float, float>& SignificanceThresholdsMap)
{
	TArray<FSignificanceSettings::FSignificanceThresholds> Thresholds;
	for (auto Threshold : SignificanceThresholdsMap)
	{
		Thresholds.Emplace(Threshold.Key,Threshold.Value);
	}
	SetSignificanceSettings(Thresholds);
}


float AMyShootCharacter::SignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo,
                                              const FTransform& Viewpoint)
{
	if (ObjectInfo->GetTag() == TEXT("Character"))
	{
		AMyShootCharacter* Character = CastChecked<AMyShootCharacter>(ObjectInfo->GetObject());
		if (Character)
		{
			const float fovScale = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetFOVAngle() / DefaultFov;
			float Distance = (Character->GetActorLocation() - Viewpoint.GetLocation()).Size();
			Distance *= fovScale;
			float Significance =  GetSignificanceByDistance(Distance);
			if (Significance<4)
			{
				if (!GetMesh()->WasRecentlyRendered(0.5f))
				{
					Significance = 0;
				}
			}
			return Significance;
		}
	}
	return 0;
}

void AMyShootCharacter::PostSignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo,
                                                 float OldSignificance, float Significance, bool bFinal)
{
	if (ObjectInfo->GetTag() == TEXT("Character"))
	{
		AMyShootCharacter* Character = CastChecked<AMyShootCharacter>(ObjectInfo->GetObject());
		if (Character)
		{
			if (Significance == 0)
			{
				SetTickInterval(0.3);
			}
			else if (Significance == 1)
			{
				SetTickInterval(0.1);
			}
			else if (Significance == 2)
			{
				SetTickInterval(0.05);
			}
			else if (Significance == 3)
			{
				SetTickInterval(0.033);
			}
			else if (Significance == 4)
			{
				SetTickInterval(1/60);
			}
		}
	}
}
void AMyShootCharacter::SetTickInterval(float interval)
{
	SetAllTickIntervals(interval);
	if (interval>0.1)
	{
		GetMesh()->SetComponentTickInterval(0.1f);
	}
	if (SyncMoveComponent)
	{
		SyncMoveComponent->SetComponentTickInterval(1/60);
	}
}

void AMyShootCharacter::SetAllTickIntervals(float interval)
{
	SetActorTickInterval(interval);
	for (auto Component : GetComponents())
	{
		if (Component)
		{
			Component->SetComponentTickInterval(interval);
		}
	}
}
USkeletalMeshComponent* AMyShootCharacter::GetRightWeaponMesh() const
{
	return GetRightHandHoldWeaponMesh();
}

USkeletalMeshComponent* AMyShootCharacter::GetLeftWeaponMesh() const
{
	return GetLeftHandHoldWeaponMesh();
}

void AMyShootCharacter::OnNearWeaponHit(const FHitResult& HitResult,const FName& HitSocketName)const
{
	OnNearWeaponHitEvent.Broadcast(HitResult, HitSocketName);
}


bool AMyShootCharacter::IsRotationChanged(const FRotator CurrentRotator) const
{
	return FMath::Abs(CurrentRotator.Pitch - LastRotator.Pitch) >= Min_Delta_Pitch || FMath::Abs(CurrentRotator.Yaw - LastRotator.Yaw) >= Min_Delta_Yaw ||
	   	   FMath::Abs(CurrentRotator.Roll - LastRotator.Roll) >= Min_Delta_Roll;	
};
UAbilitySystemComponent* AMyShootCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

FGameplayAbilitySpecHandle AMyShootCharacter::K2_GiveAbility(TSubclassOf<UCustomGameplayAbility> Ability,
	int32 InputID, int32 Lv)
{
	return AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, Lv, InputID, this));
}

void AMyShootCharacter::AddAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || AbilitySystemComponent->bAbilitiesGiven)
	{
		return;
	}
	K2_AddAbilities();
	AbilitySystemComponent->bAbilitiesGiven = true;
}

void AMyShootCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	ACustomPlayerState* PS = GetPlayerState<ACustomPlayerState>();
	if (PS)
	{
		AbilitySystemComponent = Cast<UCustomAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
		CombatAttributeSet = PS->GetCombatAttributeSet();
		
		CombatAttributeSet->OnAttributeChange.AddUObject(this, &AMyShootCharacter::OnAttributeChangeCallback);
		OnAttributeSetInitFinish.Broadcast();
		if ( !bASCBindInput && InputComponent)
		{
			AbilitySystemComponent->BindToInputComponent(InputComponent);
			bASCBindInput = true;
		}
	}
}

void AMyShootCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ACustomPlayerState* PS = GetPlayerState<ACustomPlayerState>();
	if (PS)
	{
		AbilitySystemComponent = Cast<UCustomAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
		CombatAttributeSet = PS->GetCombatAttributeSet();
		SetOwner(NewController);
		InitCharacter();
		AttributeSetInitFinish();

		AbilitySystemComponent->OnGameplayEffectAppliedDelegateToSelf.RemoveAll(this);
		AbilitySystemComponent->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&AMyShootCharacter::OnGEAppliedToSelf);
		CombatAttributeSet->OnOutOfHealth.AddUObject(this,&AMyShootCharacter::OnDie);

		CombatAttributeSet->OnAttributeChange.AddUObject(this, &AMyShootCharacter::OnAttributeChangeCallback);
	}
}

float AMyShootCharacter::GetHealth()
{
	if (!CombatAttributeSet)
	{
		return 0.f;
	}
	
	return CombatAttributeSet->GetHealth();
}

float AMyShootCharacter::GetMaxHealth()
{
	if (!CombatAttributeSet)
	{
		return 0.f;
	}
	
	return CombatAttributeSet->GetMaxHealth();
}

float AMyShootCharacter::GetAttributeValueByName(FName AttributeName)
{
	if (!CombatAttributeSet)
	{
		return 0.f;
	}

	FGameplayAttribute Attribute = UCustomAttributeSet::GetPropertyByName(UCombatAttributeSet::StaticClass(),AttributeName);
	return Attribute.GetNumericValue(CombatAttributeSet);
}

void AMyShootCharacter::DoBindInputComponent()
{
	if (InputComponent == nullptr)
	{
		InputComponent = CreatePlayerInputComponent();
		if (InputComponent)
		{
			SetupPlayerInputComponent(InputComponent);
			InputComponent->RegisterComponent();
			if (UInputDelegateBinding::SupportsInputDelegate(GetClass()))
			{
				InputComponent->bBlockInput = bBlockInput;
				UInputDelegateBinding::BindInputDelegates(GetClass(), InputComponent);
			}
		}
	}
}

void AMyShootCharacter::OnAttributeChangeCallback(FGameplayAttribute Attribute, float PrevValue, float AfterValue)
{
	if (CombatAttributeSet)
	{
		if (OnAttributeChange.IsBound())
		{
			OnAttributeChange.Broadcast(Attribute.AttributeName,PrevValue,AfterValue);
		}
	}
}

void AMyShootCharacter::InitAttributesFromLua(TMap<FString,float> InAttributes)
{
	if(CombatAttributeSet)
	{
		for(auto It : InAttributes)
		{
			FGameplayAttributeData* ValueField = nullptr;
			if (FStructProperty* TargetField = FindFProperty<FStructProperty>(UCombatAttributeSet::StaticClass(), *It.Key))
			{
				ValueField = TargetField->ContainerPtrToValuePtr<FGameplayAttributeData>(CombatAttributeSet);
			}

			if (ValueField)
			{
				ValueField->SetBaseValue(It.Value);
				ValueField->SetCurrentValue(It.Value);
			}
		}
	}
	else
	{
		UE_LOG(GAME, Error, TEXT("[AMyShootCharacter::InitAttributesFromLua]Not Find CombatAttributeSet."))
	}
}

void AMyShootCharacter::OnGEAppliedToSelf(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	if (ActiveHandle.WasSuccessfullyApplied())
	{
		for(auto It : SpecApplied.ModifiedAttributes)
		{
			if (It.Attribute == CombatAttributeSet->GetDamageAttribute() && It.TotalMagnitude > 0)
			{
				static FName AttackAttributeType = TEXT("AttackAttributeType");
				static FName PlayerOrSystemAttackType = TEXT("PlayerOrSystemAttackType");
				float AttributeType = FMath::Floor(SpecApplied.GetSetByCallerMagnitude(AttackAttributeType,false,0) + 0.5f);
				float PlayerOrSystem = FMath::Floor(SpecApplied.GetSetByCallerMagnitude(PlayerOrSystemAttackType,false,0) + 0.5f);
				OnGetHurt(Source->GetAvatarActor(),AttributeType,PlayerOrSystem);
			}
		}
	}
}

void AMyShootCharacter::OnGetHurt_Implementation(AActor* FromCharacter, int DamageAttributeType, int FromPlayerOrSystem)
{
	K2_OnGetHurt(FromCharacter, DamageAttributeType, FromPlayerOrSystem);
}

void AMyShootCharacter::AbilityLocalInputPressed(int32 AbilityEnum)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(AbilityEnum);
	}
}

void AMyShootCharacter::AbilityLocalInputReleased(int32 AbilityEnum)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityLocalInputReleased(AbilityEnum);
	}
}

void AMyShootCharacter::LocalInputConfirm()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->LocalInputConfirm();
	}
}

void AMyShootCharacter::LocalInputCancel()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->LocalInputCancel();
	}
}

void AMyShootCharacter::OnDie(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec& EffectSpec, float EffectMagnitude,AActor* AvatarActor)
{
	LuaOnDie(EffectInstigator, EffectCauser, EffectSpec, EffectMagnitude,AvatarActor);
}

void AMyShootCharacter::Cheat_Invincible(bool bOpen)
{
	static FGameplayTag GT = FGameplayTag::RequestGameplayTag("Gameplay.Cheat.Invincible");
	if (bOpen)
	{
		GetAbilitySystemComponent()->SetTagMapCount(GT,1);
	}
	else
	{
		GetAbilitySystemComponent()->SetTagMapCount(GT,0);
	}
}

void AMyShootCharacter::OnReceiveAttributeChange_Implementation()
{
	
}

void AMyShootCharacter::BindLuaFunction(const UInputAction* Action, ETriggerEvent TriggerEvent, FEnhancedInputActionHandlerDynamicSignature Callback)
{
	if (InputComponent)
	{
		if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
		{
			EIC->BindAction(Action, TriggerEvent, Callback);
		}
	}
	else
	{
		UE_LOG(GAME,Warning,TEXT("[MyShootCharacter.cpp]BindLuaFunction InputComponent is NULL."))
	}
}

void AMyShootCharacter::LuaAddMappingContext(const UInputMappingContext* MappingContext, int32 Priority)
{
	const APlayerController* PC = Cast<const APlayerController>(GetController());
	if (PC)
	{
		const ULocalPlayer* LP = PC->GetLocalPlayer();
		if (LP)
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if (Subsystem)
			{
				Subsystem->AddMappingContext(MappingContext, Priority);
				return;
			}
		}
	}
	UE_LOG(GAME, Warning, TEXT("[AMyShootCharacter]LuaAddMappingContext failed."))
}

void AMyShootCharacter::LuaRemoveMappingContext(const UInputMappingContext* MappingContext)
{
	const APlayerController* PC = Cast<const APlayerController>(GetController());
	if (PC)
	{
		const ULocalPlayer* LP = PC->GetLocalPlayer();
		if (LP)
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if (Subsystem)
			{
				Subsystem->RemoveMappingContext(MappingContext);
				return;
			}
		}
	}
	UE_LOG(GAME, Warning, TEXT("[AMyShootCharacter]LuaRemoveMappingContext failed."))
}

int32 AMyShootCharacter::AddPlayerMappedKey(const FName MappingName, const FKey NewKey, bool bForceImmediately)
{
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	return Subsystem->AddPlayerMappedKey(MappingName, NewKey, bForceImmediately);
}

int32 AMyShootCharacter::RemovePlayerMappedKey(const FName MappingName, const bool bForceImmediately)
{
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	
	return Subsystem->RemovePlayerMappedKey(MappingName, bForceImmediately);
}

FKey AMyShootCharacter::GetPlayerMappedKey(const FName MappingName) const
{
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	return Subsystem->GetPlayerMappedKey(MappingName);
}

void AMyShootCharacter::RemoveAllPlayerMappedKeys(bool bForceImmediately)
{
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);
	
	return Subsystem->RemoveAllPlayerMappedKeys(bForceImmediately);
}