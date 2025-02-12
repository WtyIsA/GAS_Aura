#include "SteamSdkInterface.h"

#include "LuaManger.h"
#include "OnlineSubsystemUtils.h"
#include "Async/Async.h"
#include "Kismet/KismetSystemLibrary.h"

FSteamSdkInterface::FSteamSdkInterface()
{
	GConfig->GetString(TEXT("OnlineSubsystemSteam"), TEXT("pchIdentity"), mStrpchIdentity, GEngineIni);
	
}


bool FSteamSdkInterface::SdkIsInit()
{
	return true;
}

void FSteamSdkInterface::SteamLogin()
{
	auto SteamOnline = Online::GetSubsystem(nullptr,STEAM_SUBSYSTEM);
	if (SteamOnline)
	{
		SteamOnline->GetIdentityInterface()->GetLinkedAccountAuthToken(0, IOnlineIdentity::FOnGetLinkedAccountAuthTokenCompleteDelegate::CreateLambda([this](int errorNum, bool bSuccess, const FExternalAuthToken& token)
		{
			AsyncTask(ENamedThreads::GameThread,[errorNum,bSuccess,token, this]()
			{
				if(bSuccess)
				{
					ULuaManger::Get()->OnLoginCallBack.ExecuteIfBound(bSuccess, token.TokenString,this->mStrpchIdentity,TEXT(""));
				}
				else
				{
					ULuaManger::Get()->OnLoginCallBack.ExecuteIfBound(false, FString::Printf(TEXT("%d"),errorNum ),TEXT(""),TEXT(""));
				}
			});
		}), TCHAR_TO_UTF8(*mStrpchIdentity));
		
		
	}else
	{
		ULuaManger::Get()->OnLoginCallBack.ExecuteIfBound(false, TEXT("-1"),TEXT(""),TEXT(""));
	}
}


void FSteamSdkInterface::AutoLogin()
{
	SteamLogin();
}

void FSteamSdkInterface::ShowSdkLoginPanel()
{
	SteamLogin();
}

void FSteamSdkInterface::Login()
{
	SteamLogin();
}

void FSteamSdkInterface::GetPlatformInfo()
{
	
}


void FSteamSdkInterface::RestartApp(UObject* obj)
{
	UKismetSystemLibrary::QuitGame(obj, nullptr, EQuitPreference::Quit, true);
}

void FSteamSdkInterface::GetCountryCode()
{
	ULuaManger::Get()->OnGetCountryCodeCallback.ExecuteIfBound(true, 0, TEXT("CN"), TEXT("China"),TEXT("beijing"));
}

void FSteamSdkInterface::GetFriendChainInfo(int32 platformId)
{
	FString info(TEXT(""));
	ULuaManger::Get()->OnGetFriendChainInfo.ExecuteIfBound(true, platformId, 0,info);
}

void FSteamSdkInterface::HasOpenFriendChainPermission(int32 platformId)
{
	ULuaManger::Get()->OnGetFriendChainPermission.ExecuteIfBound(true, platformId);
}

bool FSteamSdkInterface::NetMpaAddAddress(const TArray<FString>& address, const FString& contextId)
{
	//ULuaManger::Get()->OnNetMpaAddAddressCallback.ExecuteIfBound(true,TEXT(""));
	return false;
}

void FSteamSdkInterface::GetFriends()
{
	auto SteamOnline = Online::GetSubsystem(nullptr,STEAM_SUBSYSTEM);
	if (SteamOnline)
	{
		SteamOnline->GetFriendsInterface()->ReadFriendsList(0,TEXT(""), FOnReadFriendsListComplete::CreateLambda([](int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
			{
				ULuaManger::Get()->OnGetSdkFriendCallback.ExecuteIfBound(bWasSuccessful,ErrorStr);
			}));
	}
	else
	{
		ULuaManger::Get()->OnGetSdkFriendCallback.ExecuteIfBound(false,TEXT("{}"));
	}
}

void FSteamSdkInterface::InviteFriend(const FString& friendID, const FString& inviteInfo)
{
	auto SteamOnline = Online::GetSubsystem(nullptr,STEAM_SUBSYSTEM);
	if (SteamOnline)
		SteamOnline->GetFriendsInterface()->SendInvite(0, *SteamOnline->GetIdentityInterface()->CreateUniquePlayerId(friendID), inviteInfo);	
}

void FSteamSdkInterface::SendMsgToFriend(const FString& friendID, const FString& msg)
{
	auto SteamOnline = Online::GetSubsystem(nullptr,STEAM_SUBSYSTEM);
	if (!SteamOnline)
		return;
	FShowSendMessageParams Params;// 消息不为空直接发消息。 为空就打开私聊界面
	Params.DisplayMessage = FText::FromString(msg);
	SteamOnline->GetExternalUIInterface()->ShowSendMessageToUserUI(0,*SteamOnline->GetIdentityInterface()->CreateUniquePlayerId(friendID), Params);
}


