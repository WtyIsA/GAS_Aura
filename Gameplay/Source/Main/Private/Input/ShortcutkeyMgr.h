#pragma once

#include "CoreMinimal.h"

#include "Components/Widget.h"

#include "ShortcutkeyMgr.generated.h"

struct FShortcutKeyInfo
{
	FShortcutKeyInfo(const FShortcutKeyInfo& other):inChord(other.inChord), keyEvent(other.keyEvent)
	{		
	}
	FShortcutKeyInfo(FInputChord &otherinChord, int otherEvent):inChord(otherinChord), keyEvent(otherEvent)
	{		
	}
	FInputChord	 inChord;
	int			 keyEvent;

	bool operator==(const FShortcutKeyInfo& Other) const
	{
		return (  keyEvent == Other.keyEvent && inChord == Other.inChord );
	}
};

UCLASS()
class UShortcutkeyMgr : public  UObject
{
	GENERATED_BODY()
private:
	UShortcutkeyMgr();
public:
	static  UShortcutkeyMgr* Get();
	static  void Free();
	void Clear();
	void AddExistKeyBinds();
	void AddShortcutKey(FString keyName, bool bShift, bool bAlt,bool bCtrl, bool bCmd, UObject* obj = nullptr, int inputEvent = 0);
	bool ChangeShortcutKey(FString fromkeyName,FString tokeyName, bool bShift, bool bAlt,bool bCtrl, bool bCmd, UObject* obj = nullptr, int inputEvent = 0);
	void RemoveShortcutKey(FString keyName, bool bShift, bool bAlt,bool bCtrl, bool bCmd, UObject* obj = nullptr, int inputEvent = 0);
	void RemoveObjectShortcutKeys(UObject*obj);
	void SetInputMode(bool bUiOnly, bool bShowMouse, UWidget* widget, bool bLockMouse);
	UInputComponent* GetInputComp();
	bool OnlyUiInput(){return _bOnlyUi;}

	void AddAxisKey(FString keyName, UObject* obj = nullptr);
	void RemoveAxisKey(FString keyName, UObject* obj = nullptr);

	void UpdateIntput(uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, float Force, FDateTime DeviceTimestamp, uint32 TouchpadIndex);
private:
	void OnShortCutKey(FKey fkey);
	void OnShortCutKeyRelease(FKey fkey);
	bool IsInList(FInputChord fChold, int keyEvent);
	bool IsInAxisList(FKey& fKey);
	void CheckKeyBindings();
	void CheckAxisBindings();
	void OnAxisPinchKey(float axisValue);
	void OnAxisFlickKey(float axisValue);
	void OnAxisRotateKey(float axisValue);
	TMap<UObject*, TArray<FShortcutKeyInfo>> _umgShortcuts;
	TMap<UObject*, TArray<FString>>		_umgAxisKeys;

	static  UShortcutkeyMgr* _handle;
	bool	_bOnlyUi;
};
