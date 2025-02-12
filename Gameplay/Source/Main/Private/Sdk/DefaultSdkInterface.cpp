#include "DefaultSdkInterface.h"

#include "LuaManger.h"
#include "Kismet/KismetSystemLibrary.h"

bool FDefaultSdkInterface::SdkIsInit()
{
	return true;
}

void FDefaultSdkInterface::Login()
{
	
}

void FDefaultSdkInterface::GetPlatformInfo()
{
	
}

void FDefaultSdkInterface::RestartApp(UObject* obj)
{
	UKismetSystemLibrary::QuitGame(obj, nullptr, EQuitPreference::Quit, true);
}

void FDefaultSdkInterface::GetCountryCode()
{
	ULuaManger::Get()->OnGetCountryCodeCallback.ExecuteIfBound(true, 0, TEXT("CN"), TEXT("China"),TEXT("beijing"));
}

void FDefaultSdkInterface::GetFriendChainInfo(int32 platformId)
{
	FString info(TEXT(""));
	ULuaManger::Get()->OnGetFriendChainInfo.ExecuteIfBound(true, platformId, 0,info);
}

void FDefaultSdkInterface::HasOpenFriendChainPermission(int32 platformId)
{
	ULuaManger::Get()->OnGetFriendChainPermission.ExecuteIfBound(true, platformId);
}

bool FDefaultSdkInterface::NetMpaAddAddress(const TArray<FString>& address, const FString& contextId)
{
	//ULuaManger::Get()->OnNetMpaAddAddressCallback.ExecuteIfBound(true,TEXT(""));
	return false;
}

