#if PLATFORM_ANDROID
#include "ByteSdkAndroidInterface.h"
#include "CallJaveObjectMethod.h"
#include "GPM/Public/GPMSDKMgr.h"

void FByteSdkAndroidInterface::RestartApp(UObject* obj)
{
	FCallJaveObjectMethod::CallVoidVoidMethod("RestartApplication");
}

void FByteSdkAndroidInterface::GetPlatformInfo()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("GetPlatformInfo");
}

bool FByteSdkAndroidInterface::SdkIsInit()
{
	return FCallJaveObjectMethod::CallBoolVoidMethod("GsdkIsInit");
}

void FByteSdkAndroidInterface::Login()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("Login");
}

void FByteSdkAndroidInterface::InitSdk()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("InitGsdk");
}

void FByteSdkAndroidInterface::ShowSdkLoginPanel()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("ShowSdkLoginPanel");
}

void FByteSdkAndroidInterface::OpenUserCenter()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("OpenUserCenter");
}

void FByteSdkAndroidInterface::RealNameVerify(const FString& url,bool bMustSuccess)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = FCallJaveObjectMethod::GetJavaMethod(Env,"RealNameVerify", "(Ljava/lang/String;Z)V");
		if(MethonId != 0)
		{
			auto javaurl = FJavaHelper::ToJavaString(Env, url);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javaurl, bMustSuccess);
		}		
	}
}

void FByteSdkAndroidInterface::GetAntiAddictionState()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("GetAntiAddictionState");
}

void FByteSdkAndroidInterface::LoginNoUi(int nType)
{
	FCallJaveObjectMethod::CallVoidIntMethod("LoginNoUi", nType);
}
void FByteSdkAndroidInterface::GuestLogin()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("GuestLogin");
}

void FByteSdkAndroidInterface::VerifyActivationCode(const FString& strCode)
{
	FCallJaveObjectMethod::CallVoidOneStringMethod("VerifyActivationCode", strCode);
}

void FByteSdkAndroidInterface::Logout()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("Logout");
}
void FByteSdkAndroidInterface::GetHistoryAccounts()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("GetHistoryAccounts");
}

void FByteSdkAndroidInterface::DeleteHistoryAccount(uint64 userid)
{
	FCallJaveObjectMethod::CallVoidLongMethod("DeleteHistoryAccount", userid);
}

void FByteSdkAndroidInterface::SwitchAccount(uint64 userid)
{
	FCallJaveObjectMethod::CallVoidLongMethod("SwitchAccount", userid);
}
FString FByteSdkAndroidInterface::GetLatestUserInfo()
{
	return FCallJaveObjectMethod::CallStringVoidMethod("GetLatestUserInfo");
}
bool FByteSdkAndroidInterface::IsLogin()
{
	return FCallJaveObjectMethod::CallBoolVoidMethod("IsLogin");
}
void FByteSdkAndroidInterface::SetDebug(bool bEnable)
{
	FCallJaveObjectMethod::CallVoidBoolMethod("SetDebug", bEnable);
}
void FByteSdkAndroidInterface::SetSandboxEnable(bool bEnable)
{
	FCallJaveObjectMethod::CallVoidBoolMethod("setSandboxEnable", bEnable);
}

bool FByteSdkAndroidInterface::IsSandbox()
{
	return FCallJaveObjectMethod::CallBoolVoidMethod("IsSandbox");
}
bool FByteSdkAndroidInterface::IsEmulator()
{
	return FCallJaveObjectMethod::CallBoolVoidMethod("IsEmulator");
}

double FByteSdkAndroidInterface::GetBatteryLevel()
{
	return FCallJaveObjectMethod::CallDoubleVoidMethod("GetBatteryLevel");
}

bool FByteSdkAndroidInterface::MobileIsCharging()
{
	return FCallJaveObjectMethod::CallBoolVoidMethod("isCharging");
}

bool FByteSdkAndroidInterface::CanAutoLogin()
{
	return FCallJaveObjectMethod::CallBoolVoidMethod("CanAutoLogin");
}
void FByteSdkAndroidInterface::AutoLogin()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("AutoLogin");
}
void FByteSdkAndroidInterface::AccountBind(int nLoginType)
{
	FCallJaveObjectMethod::CallVoidIntMethod("AccountBind", nLoginType);
}
void FByteSdkAndroidInterface::AccountUnBind(int nLoginType)
{
	FCallJaveObjectMethod::CallVoidIntMethod("AccountUnBind", nLoginType);
}

void FByteSdkAndroidInterface::AccountForceRebend(int nAccountType, int force)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = FCallJaveObjectMethod::GetJavaMethod(Env,"AccountForceRebend", "(II)V");
		if(MethonId != 0)
		{
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,nAccountType, force);
		}		
	}
}
bool FByteSdkAndroidInterface::CheckProtocolVersion()
{
	return FCallJaveObjectMethod::CallBoolVoidMethod("CheckProtocolVersion");
}
void FByteSdkAndroidInterface::SetUserHasAgreedProtocolsUpdate()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("SetUserHasAgreedProtocolsUpdate");
}
void FByteSdkAndroidInterface::Report(const FString& eventName, const FString& key, const FString& Value, const FString& extraData)
{
	FCallJaveObjectMethod::CallVoidFourStringsMethod("Report", eventName,key, Value, extraData);
}

void FByteSdkAndroidInterface::Report(const FString& eventName, const FString& jsonContent)
{
	FCallJaveObjectMethod::CallVoidTwoStringsMethod("Report", eventName,jsonContent);
}

void FByteSdkAndroidInterface::UploadFile(const FString& filePath,uint64 roleId,const FString& roleName)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = FCallJaveObjectMethod::GetJavaMethod(Env,"UploadFile", "(Ljava/lang/String;JLjava/lang/String;)V");
		if(MethonId != 0)
		{
			auto javafilePath = FJavaHelper::ToJavaString(Env, filePath);
			auto javaroleName = FJavaHelper::ToJavaString(Env, roleName);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javafilePath, roleId, *javaroleName);
		}		
	}
}

void FByteSdkAndroidInterface::UploadException(const FString&  name, const FString&  reason, const FString&  stackTrace)
{
	FCallJaveObjectMethod::CallVoidThreeStringsMethod("UploadException", name,reason, stackTrace);
}

void FByteSdkAndroidInterface::JudgeAgeGate()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("JudgeAgeGate");
}
void FByteSdkAndroidInterface::ModifyAgeGateStatus(bool bEnable)
{
	FCallJaveObjectMethod::CallVoidBoolMethod("ModifyAgeGateStatus", bEnable);
}
bool FByteSdkAndroidInterface::QueryAgeGateStatus()
{
	return FCallJaveObjectMethod::CallBoolVoidMethod("QueryAgeGateStatus");
}

FString FByteSdkAndroidInterface::GetSdkOpenId()
{
	return FCallJaveObjectMethod::CallStringVoidMethod("GetSdkOpenId");
}

void FByteSdkAndroidInterface::GetUserAgreementUrlAndPrivacyPolicy(FString& strUserAgreementUrl,FString& strPrivacyPolicy)
{
	strUserAgreementUrl = FCallJaveObjectMethod::CallStringVoidMethod("GetUserAgreementUrl");
	strPrivacyPolicy = FCallJaveObjectMethod::CallStringVoidMethod("GetPrivacyPolicy");
}

int  FByteSdkAndroidInterface::GetCurrentNetState()
{
	return FCallJaveObjectMethod::CallIntVoidMethod("GetCurrentNetState");
}

void FByteSdkAndroidInterface::OpenUrl(const FString& title, const FString& url, const FString& extraData)
{
	FCallJaveObjectMethod::CallVoidThreeStringsMethod("OpenUrl",title, url,  extraData);
}

void FByteSdkAndroidInterface::CDKeyExchange(const FString& cdkey, const FString& roleid, const FString& serverid)
{
	FCallJaveObjectMethod::CallVoidThreeStringsMethod("CDKeyExchange",cdkey, roleid,  serverid);
}

void FByteSdkAndroidInterface::NetDiagnosis(const  FString& target, int timeout)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = FCallJaveObjectMethod::GetJavaMethod(Env,"NetDiagnosis", "(Ljava/lang/String;I)V");
		if(MethonId != 0)
		{
			auto javatarget = FJavaHelper::ToJavaString(Env, target);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javatarget, timeout);
		}		
	}
}

void FByteSdkAndroidInterface::AddLocalNotification(const FString& title,const FString& content, uint64 nDelaySeconds)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = FCallJaveObjectMethod::GetJavaMethod(Env,"addLocalNotification", "(Ljava/lang/String;Ljava/lang/String;J)V");
		if(MethonId != 0)
		{
			auto javatitle = FJavaHelper::ToJavaString(Env, title);
			auto javaContent = FJavaHelper::ToJavaString(Env, content);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javatitle,*javaContent, nDelaySeconds);
		}		
	}
}

void FByteSdkAndroidInterface::FetchNotice(int nScene, const FString& savePath, const FString& strLanguage, const FString& otherParams)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = FCallJaveObjectMethod::GetJavaMethod(Env,"fetchNotices", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
		if(MethonId != 0)
		{
			auto javasavePath = FJavaHelper::ToJavaString(Env, savePath);
			auto javastrLanguage = FJavaHelper::ToJavaString(Env, strLanguage);
			auto javaotherParams = FJavaHelper::ToJavaString(Env, otherParams);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,nScene,*javasavePath,*javastrLanguage,*javaotherParams);
		}		
	}
}

void FByteSdkAndroidInterface::LogSceneStart(const FString& sceneName)
{
	GPMSDKMgr::GetInstance().GetSDK().LogSceneStart(TCHAR_TO_UTF8(*sceneName));
}
void FByteSdkAndroidInterface::LogSceneLoaded()
{
	GPMSDKMgr::GetInstance().GetSDK().LogSceneLoaded();
}
void FByteSdkAndroidInterface::LogSceneEnd()
{
	GPMSDKMgr::GetInstance().GetSDK().LogSceneEnd();
}

void FByteSdkAndroidInterface::SetUserInfoForSecureService(const FString& roleId,const FString& serverId)
{
	FCallJaveObjectMethod::CallVoidTwoStringsMethod("SetUserInfoForSecureService", roleId,serverId);
}

bool FByteSdkAndroidInterface::IsPermissionGranted(const FString& strPermission)
{
	return FCallJaveObjectMethod::CallBoolFStringMethod("IsPermissionGranted", strPermission);
}
void FByteSdkAndroidInterface::OpenAppSetting()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("OpenAppSetting");
}

bool FByteSdkAndroidInterface::RequestPermission(const FString& strPermission, bool bFirstRequest)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool bIsOptional = false;
		jmethodID MethonId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "RequestPermission", "(Ljava/lang/String;Z)Z", bIsOptional);
		if(MethonId != 0)
		{
			auto javastrPermission = FJavaHelper::ToJavaString(Env, strPermission);
			return FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javastrPermission, bFirstRequest);
		}		
	}
	return true;
}

bool FByteSdkAndroidInterface::IsNotificationsAllowed(const FString& strChannelName)
{
	return FCallJaveObjectMethod::CallBoolFStringMethod("IsNotificationsAllowed", strChannelName);
}

void FByteSdkAndroidInterface::GetCountryCode()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("GetCountryCode");
}

void FByteSdkAndroidInterface::ChangeLanguage(const FString& langType)
{
	FCallJaveObjectMethod::CallVoidOneStringMethod("ChangeLanguage", langType);
}

void FByteSdkAndroidInterface::ShowRatingUi()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("ShowRatingUi");
}

void FByteSdkAndroidInterface::ShowRatingCustomUi()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("ShowRatingCustomUi");
}
void FByteSdkAndroidInterface::DismissRatingCustomUi()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("DismissRatingCustomUi");
}
void FByteSdkAndroidInterface::OnRatingPraiseBtnClick()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("OnRatingPraiseBtnClick");
}
void FByteSdkAndroidInterface::OnRatingFeedbackBtnClick()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("OnRatingFeedbackBtnClick");
}

bool FByteSdkAndroidInterface::IsBsdkPay()
{
	return FCallJaveObjectMethod::CallBoolVoidMethod("IsBsdkPay");
}
void FByteSdkAndroidInterface::Pay(const FString& productId, int goodType, int amount, const FString& currency,
	const FString& roleId,const FString& roleName, int roleLevel, int roleVipLevel,
	const FString& serverId,const FString& extInfo)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = FCallJaveObjectMethod::GetJavaMethod(Env,"Pay", "(Ljava/lang/String;IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;IILjava/lang/String;Ljava/lang/String;)V");
		if(MethonId != 0)
		{
			auto javaproductId = FJavaHelper::ToJavaString(Env, productId);
			auto javacurrency = FJavaHelper::ToJavaString(Env, currency);
			auto javaroleId = FJavaHelper::ToJavaString(Env, roleId);
			auto javaroleName = FJavaHelper::ToJavaString(Env, roleName);
			auto javaserverId = FJavaHelper::ToJavaString(Env, serverId);
			auto javaextInfo = FJavaHelper::ToJavaString(Env, extInfo);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javaproductId,goodType,amount,
				*javacurrency, *javaroleId,*javaroleName,roleLevel,roleVipLevel,*javaserverId,*javaextInfo);
		}		
	}
}
void FByteSdkAndroidInterface::QueryProducts(const FString& productId)
{
	FCallJaveObjectMethod::CallVoidOneStringMethod("QueryProducts", productId);
}
void FByteSdkAndroidInterface::QueryGoods(const FString& roleId,const FString& serverId, bool bCurrency)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = FCallJaveObjectMethod::GetJavaMethod(Env,"QueryGoods", "(Ljava/lang/String;Ljava/lang/String;Z)V");
		if(MethonId != 0)
		{
			auto javaparam1 = FJavaHelper::ToJavaString(Env, roleId);
			auto javaparam2 = FJavaHelper::ToJavaString(Env, serverId);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javaparam1, *javaparam2,bCurrency);
		}		
	}
}
void FByteSdkAndroidInterface::SetMinorLimit(const FString& country,const FString& content)
{
	FCallJaveObjectMethod::CallVoidTwoStringsMethod("SetMinorLimit", country,content);
}
void FByteSdkAndroidInterface::GetMinorLimit(const FString& country)
{
	FCallJaveObjectMethod::CallVoidOneStringMethod("GetMinorLimit", country);
}

void FByteSdkAndroidInterface::HasOpenFriendChainPermission(int32  platformId)
{
	FCallJaveObjectMethod::CallVoidIntMethod("HasOpenFriendChainPermission", platformId);
}
void FByteSdkAndroidInterface::GetFriendChainInfo(int32 platformId)
{
	FCallJaveObjectMethod::CallVoidIntMethod("GetFriendChainInfo", platformId);
}

FString FByteSdkAndroidInterface::GetChannelOp()
{
	return FCallJaveObjectMethod::CallStringVoidMethod("GetChannelOp");
}

void FByteSdkAndroidInterface::ChannelGeneralAction(int32 code)
{
	FCallJaveObjectMethod::CallVoidIntMethod("ChannelGeneralAction", code);
}

bool FByteSdkAndroidInterface::UnZip(const FString& filePath,const FString& destDirPath, bool bSoFile)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool bIsOptional = false;
		jmethodID MethonId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "UnZip", "(Ljava/lang/String;Ljava/lang/String;Z)Z", bIsOptional);
		if(MethonId != 0)
		{
			auto javafilePath = FJavaHelper::ToJavaString(Env, filePath);
			auto javadestDir = FJavaHelper::ToJavaString(Env, destDirPath);
			return FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javafilePath, *javadestDir, bSoFile);
		}		
	}
	return false;
}

void FByteSdkAndroidInterface::NetMpaStart(const FString& startLog)
{
	FCallJaveObjectMethod::CallVoidOneStringMethod("NetMpaStart",startLog);
}

bool FByteSdkAndroidInterface::NetMpaAddAddress(const TArray<FString>& address,const FString& contextId)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool bIsOptional = false;
		jmethodID MethonId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "NetMpaAddAddress", "([Ljava/lang/String;Ljava/lang/String;)Z", bIsOptional);
		if(MethonId == 0)
		{
			return false;
		}
		auto AddressJavaArray = NewScopedJavaObject(Env, (jobjectArray)Env->NewObjectArray(address.Num(), FJavaWrapper::JavaStringClass, NULL));
		if (AddressJavaArray)
		{
			for (uint32 Param = 0; Param < address.Num(); Param++)
			{
				auto StringValue = FJavaHelper::ToJavaString(Env, address[Param]);
				Env->SetObjectArrayElement(*AddressJavaArray, Param, *StringValue);
			}
			auto javacontextId = FJavaHelper::ToJavaString(Env, contextId);
			return FJavaWrapper::CallBooleanMethod(Env, FJavaWrapper::GameActivityThis, MethonId, *AddressJavaArray,*javacontextId);
		}
	}
	return false;
}

void FByteSdkAndroidInterface::NetMpaStop(const FString& stopLog)
{
	FCallJaveObjectMethod::CallVoidOneStringMethod("NetMpaStop",stopLog);
}

void FByteSdkAndroidInterface::AddUploadLogfileFullPaths(const TArray<FString>& files)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		bool bIsOptional = false;
		jmethodID MethonId = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AddUploadLogfileFullPaths", "([Ljava/lang/String;)V", bIsOptional);
		if(MethonId == 0)
		{
			UE_LOG(LogTemp,Display,TEXT("[sdk]AddUploadLogfileFullPaths not find"));
			return;
		}
		auto filesJavaArray = NewScopedJavaObject(Env, (jobjectArray)Env->NewObjectArray(files.Num(), FJavaWrapper::JavaStringClass, NULL));
		if (filesJavaArray)
		{
			for (uint32 Param = 0; Param < files.Num(); Param++)
			{
				auto StringValue = FJavaHelper::ToJavaString(Env, files[Param]);
				Env->SetObjectArrayElement(*filesJavaArray, Param, *StringValue);
			}
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId, *filesJavaArray);
		}
	}	
}
void FByteSdkAndroidInterface::AddUploadLogfileDirPath(const FString& fileDir)
{
	FCallJaveObjectMethod::CallVoidOneStringMethod("AddUploadLogfileDirPath",fileDir);
}

#endif //end  PLATFORM_ANDROID