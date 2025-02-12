#include "ShortcutkeyMgr.h"

#include "LuaManger.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"

UShortcutkeyMgr* UShortcutkeyMgr::_handle = nullptr;
UShortcutkeyMgr::UShortcutkeyMgr()
{
	_bOnlyUi = false;
}

UShortcutkeyMgr* UShortcutkeyMgr::Get()
{
	if(_handle == nullptr)
	{
		_handle = NewObject<UShortcutkeyMgr>(ULuaManger::m_pGame);
		_handle->AddToRoot();       
	}
	return _handle;
}

void UShortcutkeyMgr::AddShortcutKey(FString keyName, bool bShift, bool bAlt, bool bCtrl, bool bCmd, UObject* obj, int inputEvent)
{
	if(obj == nullptr)
		obj = this;
	
	FInputChord InputChord(FKey(*keyName), bShift, bCtrl, bAlt, bCmd );
	FShortcutKeyInfo keyInfo(InputChord ,inputEvent);
	TArray<FShortcutKeyInfo>& shortcuts = _umgShortcuts.FindOrAdd(obj);
	for(auto item: shortcuts)
	{
		if(keyInfo == item)
			return;
	}
	shortcuts.Add(keyInfo);
	UInputComponent* InputComponent = GetInputComp();
	if(InputComponent == nullptr)
		return;;
	if(inputEvent == IE_Pressed)
	{
		FInputKeyBinding KB(InputChord, IE_Pressed);
		KB.KeyDelegate.BindDelegate(this, &UShortcutkeyMgr::OnShortCutKey);
		InputComponent->KeyBindings.Emplace(MoveTemp(KB));
	}
	else if(inputEvent == IE_Released)
	{
		FInputKeyBinding KB(InputChord, IE_Released);
		KB.KeyDelegate.BindDelegate(this, &UShortcutkeyMgr::OnShortCutKeyRelease);
		InputComponent->KeyBindings.Emplace(MoveTemp(KB));
	}
	
}

bool UShortcutkeyMgr::ChangeShortcutKey(FString fromkeyName,FString tokeyName, bool bShift, bool bAlt,bool bCtrl, bool bCmd, UObject* obj, int inputEvent)
{
	if(obj == nullptr)
		obj = this;
	TArray<FShortcutKeyInfo>* shortcuts = _umgShortcuts.Find(obj);
	if(shortcuts == nullptr)
		return false;
	bool bFind = false;
	FInputChord InputChord(FKey(*tokeyName), bShift, bCtrl, bAlt, bCmd );
	FShortcutKeyInfo keyInfo(InputChord ,inputEvent);
	for(int i = 0; i < shortcuts->Num();i++)
	{
		auto item = shortcuts->GetData()[i];
		if(item.inChord.Key.ToString() == fromkeyName && item.keyEvent == inputEvent)
		{
			bFind = true;
			shortcuts->GetData()[i] = keyInfo;
			break;
		}
	}
	if(!bFind)
		return false;
	UInputComponent* InputComponent = GetInputComp();
	if(InputComponent == nullptr)
		return false;
	for(int i = 0; i < InputComponent->KeyBindings.Num(); i++)
	{
		auto &item = InputComponent->KeyBindings[i];
		if(item.KeyEvent == inputEvent && item.Chord.Key.ToString() == fromkeyName)
		{
			item.Chord = InputChord;
			return  true;
		}
	}
	return  false;
}

void UShortcutkeyMgr::AddAxisKey(FString keyName, UObject* obj)
{
#if PLATFORM_WINDOWS
	return;
#endif

	if(obj == nullptr)
		obj = this;
	TArray<FString>& shortcuts = _umgAxisKeys.FindOrAdd(obj);
	for(int i =0;i < shortcuts.Num(); i++)
	{
		if(keyName == shortcuts[i])
		{
			return;
		}
	}
	UInputComponent* InputComponent = GetInputComp();
	if(InputComponent == nullptr)
		return;;
	shortcuts.Add(keyName);
	
	if(keyName == TEXT("Gesture_Pinch"))
		InputComponent->BindAxisKey(EKeys::Gesture_Pinch, this, &UShortcutkeyMgr::OnAxisPinchKey);
	else if(keyName == TEXT("Gesture_Flick"))
		InputComponent->BindAxisKey(EKeys::Gesture_Flick, this, &UShortcutkeyMgr::OnAxisFlickKey);
	else if(keyName == TEXT("Gesture_Rotate"))
		InputComponent->BindAxisKey(EKeys::Gesture_Rotate, this, &UShortcutkeyMgr::OnAxisRotateKey);
}

void UShortcutkeyMgr::RemoveAxisKey(FString keyName, UObject* obj)
{
	if(obj == nullptr)
		obj = this;
	TArray<FString>& shortcuts = _umgAxisKeys.FindOrAdd(obj);
	for(int i =0;i < shortcuts.Num(); i++)
	{
		if(keyName == shortcuts[i])
		{
			shortcuts.RemoveAt(i);
			break;
		}
	}
	CheckAxisBindings();
}

void UShortcutkeyMgr::RemoveShortcutKey(FString keyName, bool bShift, bool bAlt, bool bCtrl, bool bCmd, UObject* obj, int inputEvent )
{
	if(obj == nullptr)
		obj = this;
	FInputChord InputChord(FKey(*keyName), bShift, bCtrl, bAlt, bCmd );
	FShortcutKeyInfo keyInfo(InputChord ,inputEvent);
	TArray<FShortcutKeyInfo>& shortcuts = _umgShortcuts.FindOrAdd(obj);
	for(int i =0;i < shortcuts.Num(); i++)
	{
		if(keyInfo == shortcuts[i])
		{
			shortcuts.RemoveAt(i);
			break;
		}
	}
	CheckKeyBindings();
}

void UShortcutkeyMgr::RemoveObjectShortcutKeys(UObject* obj)
{
	_umgShortcuts.Remove(obj);
	_umgAxisKeys.Remove(obj);
	CheckKeyBindings();
	CheckAxisBindings();
}

void UShortcutkeyMgr::SetInputMode(bool bUiOnly, bool bShowMouse, UWidget* widget, bool bLockMouse)
{
	//APlayerController * p =  UGameplayStatics::GetPlayerController(ULuaManger::Get()->m_pGame->GetWorld(), 0);
	APlayerController * p = ULuaManger::Get()->m_pGame->GetFirstLocalPlayerController(ULuaManger::Get()->m_pGame->GetWorld());
	if(p)
	{
		_bOnlyUi = bUiOnly;
		if (bUiOnly)
			UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(p, widget, EMouseLockMode::DoNotLock);
		else
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(p);
		p->SetShowMouseCursor(bShowMouse);
		ULuaManger::m_pGame->GetGameViewportClient()->SetIgnoreInput(false);
	}
}

void UShortcutkeyMgr::OnShortCutKey(FKey fkey)
{
	bool bShift = false;
	bool bAlt = false;
	bool bCtrl = false;
	bool bCmd = false;
	APlayerController * pControl = ULuaManger::Get()->m_pGame->GetFirstLocalPlayerController(ULuaManger::Get()->m_pGame->GetWorld());
	if(pControl)
	{
		if(pControl->IsInputKeyDown(EKeys::LeftShift) || pControl->IsInputKeyDown(EKeys::RightShift))
			bShift = true;
		if(pControl->IsInputKeyDown(EKeys::LeftAlt) || pControl->IsInputKeyDown(EKeys::RightAlt))
			bAlt = true;
		if(pControl->IsInputKeyDown(EKeys::LeftControl) || pControl->IsInputKeyDown(EKeys::RightControl))
			bCtrl = true;		
		if (pControl->IsInputKeyDown(EKeys::LeftCommand) || pControl->IsInputKeyDown(EKeys::RightCommand))
			bCmd = true;
	}
	
	ULuaManger::Get()->OnShortcutKey(this ,fkey.GetFName().ToString(), bShift, bAlt, bCtrl, bCmd,IE_Pressed);
	
}

void UShortcutkeyMgr::OnShortCutKeyRelease(FKey fkey)
{
	bool bShift = false;
	bool bAlt = false;
	bool bCtrl = false;
	bool bCmd = false;
	APlayerController * pControl = ULuaManger::Get()->m_pGame->GetFirstLocalPlayerController(ULuaManger::Get()->m_pGame->GetWorld());
	if(pControl)
	{
		if(pControl->IsInputKeyDown(EKeys::LeftShift) || pControl->IsInputKeyDown(EKeys::RightShift))
			bShift = true;
		if(pControl->IsInputKeyDown(EKeys::LeftAlt) || pControl->IsInputKeyDown(EKeys::RightAlt))
			bAlt = true;
		if(pControl->IsInputKeyDown(EKeys::LeftControl) || pControl->IsInputKeyDown(EKeys::RightControl))
			bCtrl = true;		
	}
	
	ULuaManger::Get()->OnShortcutKey(this ,fkey.GetFName().ToString(), bShift, bAlt, bCtrl, bCmd, IE_Released);
	
}

bool UShortcutkeyMgr::IsInList(FInputChord fChold, int keyEvent)
{
	for(auto item : _umgShortcuts)
	{
		for(auto input:item.Value)
		{
			if(input.keyEvent == keyEvent && input.inChord == fChold)
            			return true;
		}
		
	}
	return  false;
}

bool UShortcutkeyMgr::IsInAxisList(FKey& fKey)
{
	FString strKey = fKey.ToString();
	for(auto item : _umgAxisKeys)
	{
		for(auto input:item.Value)
		{
			if(input == strKey)
				return true;
		}
		
	}
	return  false;
}

void UShortcutkeyMgr::Clear()
{
	_umgShortcuts.Empty();
	UInputComponent* InputComponent = GetInputComp();
	if(InputComponent == nullptr)
		return;;
	for(int i = InputComponent->KeyBindings.Num() - 1; i >= 0; i--)
	{
		FInputKeyBinding& Binding = InputComponent->KeyBindings[i];
		if(Binding.KeyDelegate.GetUObject() == this)
		{
			InputComponent->KeyBindings.RemoveAt(i);
		}
	}

	_umgAxisKeys.Empty();
	for(int i = InputComponent->AxisKeyBindings.Num() - 1; i >= 0; i--)
	{
		FInputAxisKeyBinding& Binding = InputComponent->AxisKeyBindings[i];
		if(Binding.AxisDelegate.GetDelegate().GetUObject() == this)
		{
			InputComponent->AxisKeyBindings.RemoveAt(i);
		}
	}
}

void UShortcutkeyMgr::AddExistKeyBinds()
{
	UInputComponent* InputComponent = GetInputComp();
	if(InputComponent == nullptr)
		return;;
	for(auto item : _umgShortcuts)
	{
		for(auto input:item.Value)
		{
			if(input.keyEvent == IE_Pressed)
			{
				FInputKeyBinding KB(input.inChord, IE_Pressed);
				KB.KeyDelegate.BindDelegate(this, &UShortcutkeyMgr::OnShortCutKey);
				InputComponent->KeyBindings.Emplace(MoveTemp(KB));
			}
			else if(input.keyEvent == IE_Released)
			{
				FInputKeyBinding KB(input.inChord, IE_Released);
				KB.KeyDelegate.BindDelegate(this, &UShortcutkeyMgr::OnShortCutKeyRelease);
				InputComponent->KeyBindings.Emplace(MoveTemp(KB));
			}
			
		}
		
	}

	for(auto item:_umgAxisKeys)
	{
		for(auto keyName :item.Value)
		{
			if(keyName == TEXT("Gesture_Pinch"))
				InputComponent->BindAxisKey(EKeys::Gesture_Pinch, this, &UShortcutkeyMgr::OnAxisPinchKey);
			else if(keyName == TEXT("Gesture_Flick"))
				InputComponent->BindAxisKey(EKeys::Gesture_Flick, this, &UShortcutkeyMgr::OnAxisFlickKey);
			else if(keyName == TEXT("Gesture_Rotate"))
				InputComponent->BindAxisKey(EKeys::Gesture_Rotate, this, &UShortcutkeyMgr::OnAxisRotateKey);
		}
	}
}

void UShortcutkeyMgr::Free()
{
	if(_handle!=nullptr)
	{
		_handle->RemoveFromRoot();
		_handle = nullptr;
	}
}



void UShortcutkeyMgr::CheckKeyBindings()
{	
	UInputComponent* InputComponent = GetInputComp();
	if(InputComponent == nullptr)
		return;;
	
	for(int i = InputComponent->KeyBindings.Num() - 1; i >= 0; i--)
	{
		FInputKeyBinding& Binding = InputComponent->KeyBindings[i];
		if(Binding.KeyDelegate.GetUObject() == this && !IsInList(Binding.Chord, Binding.KeyEvent))
		{
			InputComponent->KeyBindings.RemoveAt(i);
		}
	}

}

void UShortcutkeyMgr::CheckAxisBindings()
{	
	UInputComponent* InputComponent = GetInputComp();
	if(InputComponent == nullptr)
		return;;
	
	for(int i = InputComponent->AxisKeyBindings.Num() - 1; i >= 0; i--)
	{
		FInputAxisKeyBinding& Binding = InputComponent->AxisKeyBindings[i];
		if(Binding.AxisDelegate.GetDelegate().GetUObject() == this && !IsInAxisList(Binding.AxisKey))
		{
			InputComponent->AxisKeyBindings.RemoveAt(i);
		}
	}

}

UInputComponent* UShortcutkeyMgr::GetInputComp()
{
	APlayerController * p = ULuaManger::Get()->m_pGame->GetFirstLocalPlayerController(ULuaManger::Get()->m_pGame->GetWorld());
	if(p == nullptr /*|| p->GetCharacter() == nullptr */)
	{
		_umgShortcuts.Empty();
		return nullptr;
	}
	UInputComponent* InputComponent = p->InputComponent;
	return  InputComponent;
}

void UShortcutkeyMgr::OnAxisPinchKey(float axisValue)
{	
	APlayerController * p = ULuaManger::Get()->m_pGame->GetFirstLocalPlayerController(ULuaManger::Get()->m_pGame->GetWorld());
	if(p == nullptr|| p->PlayerInput == nullptr)
	{
		return ;
	}
	if(p->PlayerInput->IsPressed(EKeys::Gesture_Pinch))
	{
		FVector2D touch1;
		FVector2D touch2;		
		bool bPress = true;;
		bool bPress2 = true;;
		p->GetInputTouchState(ETouchIndex::Touch1, touch1.X, touch1.Y, bPress);
		p->GetInputTouchState(ETouchIndex::Touch2, touch2.X, touch2.Y, bPress2);		
		ULuaManger::Get()->OnAxisKey(this, TEXT("Gesture_Pinch"), axisValue, touch1, touch2);
		return;
	}
	else if(p->PlayerInput->WasJustReleased(EKeys::Gesture_Pinch))
	{
		axisValue = 0.0f;
		ULuaManger::Get()->OnAxisKey(this, TEXT("Gesture_Pinch"), axisValue, FVector2D::ZeroVector, FVector2D::ZeroVector);
	}
}

void UShortcutkeyMgr::OnAxisFlickKey(float axisValue)
{
	APlayerController * p = ULuaManger::Get()->m_pGame->GetFirstLocalPlayerController(ULuaManger::Get()->m_pGame->GetWorld());
	if(p == nullptr|| p->PlayerInput == nullptr)
	{
		return ;
	}
	if(!p->PlayerInput->WasJustPressed(EKeys::Gesture_Flick))
	{
		return;
	}
	
	FVector2D touch1;
	FVector2D touch2;
	bool bPress = true;;
	bool bPress2 = true;;
	p->GetInputTouchState(ETouchIndex::Touch1, touch1.X, touch1.Y, bPress);
	p->GetInputTouchState(ETouchIndex::Touch2, touch2.X, touch2.Y, bPress2);
	
	ULuaManger::Get()->OnAxisKey(this, TEXT("Gesture_Flick"), axisValue, touch1, touch2);
}
void UShortcutkeyMgr::OnAxisRotateKey(float axisValue)
{
	APlayerController * p = ULuaManger::Get()->m_pGame->GetFirstLocalPlayerController(ULuaManger::Get()->m_pGame->GetWorld());
	if(p == nullptr|| p->PlayerInput == nullptr)
	{
		return ;
	}
	if(!p->PlayerInput->IsPressed(EKeys::Gesture_Rotate))
	{
		return;
	}
	FVector2D touch1;
	FVector2D touch2;
	bool bPress = true;;
	bool bPress2 = true;;
	p->GetInputTouchState(ETouchIndex::Touch1, touch1.X, touch1.Y, bPress);
	p->GetInputTouchState(ETouchIndex::Touch2, touch2.X, touch2.Y, bPress2);
	
	ULuaManger::Get()->OnAxisKey(this, TEXT("Gesture_Rotate"), axisValue, touch1, touch2);
}

void UShortcutkeyMgr::UpdateIntput(uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, float Force, FDateTime DeviceTimestamp, uint32 TouchpadIndex)
{
	APlayerController * p = ULuaManger::Get()->m_pGame->GetFirstLocalPlayerController(ULuaManger::Get()->m_pGame->GetWorld());
	if(p != nullptr && p->PlayerInput)
	{
		p->PlayerInput->InputTouch(Handle, Type, TouchLocation, Force, DeviceTimestamp, TouchpadIndex);
	}
}