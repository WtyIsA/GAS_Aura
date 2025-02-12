
#include "UIPanel.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LuaManger.h"
#include "Engine/GameViewportClient.h"
#include "Input/CommonUIInputTypes.h"
#include "Kismet/GameplayStatics.h"

UUIPanel::UUIPanel(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsBackHandler = true;
	bIsFocusable = true;
}

void UUIPanel::ResetZOrderInViewPort(int32 zOrder)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
		return;
	
	UGameViewportClient *GameViewportClient = GameInstance->GetGameViewportClient();
	if (!GameViewportClient)
		return;
}


int UUIPanel::HandleInstanceId = 0;

int UUIPanel::LuaRegisterUIActionBinding_Internal(const FDataTableRowHandle& InHandle, bool bDisplayInActionBar, const int InIndex)
{
	HandleInstanceId++;
	FBindUIActionArgs Args = FBindUIActionArgs(InHandle,bDisplayInActionBar, FSimpleDelegate::CreateUObject(this, &UUIPanel::CallLuaCommonUIFunction, InIndex));
	//if (!IsModal())
	//{
	//	Args.bIsPersistent = true;
	//}
	IndexToHandleMap.Add(HandleInstanceId,RegisterUIActionBinding(Args));
	return HandleInstanceId;
}

void UUIPanel::LuaRemoveActionBinding(int InIndex)
{
	if (FUIActionBindingHandle* Handle = IndexToHandleMap.Find(InIndex))
	{
		RemoveActionBinding(*Handle);
	}
}

void UUIPanel::LuaAddActionBinding(int InIndex)
{
	if (FUIActionBindingHandle* Handle = IndexToHandleMap.Find(InIndex))
	{
		AddActionBinding(*Handle);
	}
}

void UUIPanel::LuaAddMappingContext(APlayerController* PC, APawn* Pawn, const UInputMappingContext* MappingContext, int32 InPriority)
{
	if (PC)
	{
		PC->SetPawn(Pawn);
		const ULocalPlayer* LP = GetOwningLocalPlayer();
		if (LP)
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if (Subsystem)
			{
				Subsystem->AddMappingContext(MappingContext, InPriority);
				return;
			}
		}
	}
	UE_LOG(GAME, Warning, TEXT("[UUIPanel]AddMappingContext failed. PanelName : %s"), *GetName())
}

void UUIPanel::LuaRemoveMappingContext(const APlayerController* PC, const UInputMappingContext* MappingContext)
{
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
	UE_LOG(GAME, Warning, TEXT("[UUIPanel]RemoveMappingContext failed. PanelName : %s"), *GetName())
}

void UUIPanel::LuaBindInputAction_Internal(const APlayerController* PC, const UInputAction* Action, ETriggerEvent TriggerEvent, FEnhancedInputActionHandlerDynamicSignature Callback)
{
	if (PC)
	{
		if (UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PC->InputComponent))
		{
			EIC->BindAction(Action, TriggerEvent, Callback);
			return;
		}
	}
	UE_LOG(GAME, Warning, TEXT("[UUIPanel]BindInputAction failed. PanelName : %s"), *GetName())
}

void UUIPanel::LuaResetInputActions(const APlayerController* PC)
{
	if (PC)
	{
		if (UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PC->InputComponent))
		{
			EIC->ClearActionEventBindings();
			return;
		}
	}
	UE_LOG(GAME, Warning, TEXT("[UUIPanel]ResetInputActions failed. PanelName : %s"), *GetName())
}
