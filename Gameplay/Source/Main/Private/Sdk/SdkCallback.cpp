#include "SdkCallback.h"

#include "LuaManger.h"
#include "Sdk/VoiceInterface.h"
/*
*    public static final  int  _accountLogin = 1;
public static final  int  _accountLogout = 2;
public static final  int  _accountSwitch = 3;
public static final  int  _accountBind = 4;
public static final  int  _accountUnBind = 5;
 */


#if PLATFORM_ANDROID


#define BYTESDKACCOUNT_LOGIN		1
#define BYTESDKACCOUNT_LOGOUT		2
#define BYTESDKACCOUNT_SWITCH		3
#define BYTESDKACCOUNT_BIND			4
#define BYTESDKACCOUNT_UNBIND		5

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_SetPlatformInfo(JNIEnv* Env,jobject thiz, jint platformId, jstring projectId, jstring channelId, jstring appChannelId)
{
	FString ueprojectId = FJavaHelper::FStringFromParam(Env, projectId);
	FString uechannelId = FJavaHelper::FStringFromParam(Env, channelId);
	FString ueappChannelId = FJavaHelper::FStringFromParam(Env, appChannelId);
	ULuaManger::Get()->SetPlatformInfo(platformId, ueprojectId,uechannelId,ueappChannelId);
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_AccountCallback(JNIEnv* Env,jobject thiis,jint nType, jboolean bSuccess, jstring strToken, jstring strData, jstring strExtenData)
{
	FString ueStrToken = FJavaHelper::FStringFromParam(Env, strToken);
	FString ueStrData = FJavaHelper::FStringFromParam(Env, strData);
	FString ueStrExtenData = FJavaHelper::FStringFromParam(Env, strExtenData);
	AsyncTask(ENamedThreads::GameThread,[nType, bSuccess, ueStrToken, ueStrData,ueStrExtenData]()
	{
		switch (nType)
		{
		case BYTESDKACCOUNT_LOGIN:
			ULuaManger::Get()->OnLoginCallBack.ExecuteIfBound(bSuccess, ueStrToken,ueStrData, ueStrExtenData);
			break;
		case BYTESDKACCOUNT_LOGOUT:
			ULuaManger::Get()->OnLogOutCallBack.ExecuteIfBound(bSuccess, ueStrToken,ueStrData, ueStrExtenData);
			break;
		case BYTESDKACCOUNT_SWITCH:
			ULuaManger::Get()->OnSwitchAccountCallBack.ExecuteIfBound(bSuccess, ueStrToken,ueStrData, ueStrExtenData);
            break;
		case BYTESDKACCOUNT_BIND:
			ULuaManger::Get()->OnAccountBindCallBack.ExecuteIfBound(bSuccess, ueStrToken,ueStrData, ueStrExtenData);
            break;
		case BYTESDKACCOUNT_UNBIND:
			ULuaManger::Get()->OnAccountUnBindCallBack.ExecuteIfBound(bSuccess, ueStrToken,ueStrData, ueStrExtenData);
            break;
		}
		
	});
}

void ConventJavelistToTArray(JNIEnv* Env, jobject javalist,TArray<FString>& array)
{
	jclass jcs_alist = Env->GetObjectClass(javalist);
	jmethodID alist_get = Env->GetMethodID(jcs_alist, "get", "(I)Ljava/lang/Object;");
	jmethodID alist_size = Env->GetMethodID(jcs_alist, "size", "()I");
	jint len = Env->CallIntMethod(javalist, alist_size);	
	for (int i = 0; i < len; i++)
	{
		jstring javaData = (jstring)Env->CallObjectMethod(javalist, alist_get, i);
		FString ueData = FJavaHelper::FStringFromParam(Env, javaData);
		array.Add(ueData);
	}
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_HistoryAccountsCallback(JNIEnv* Env,jobject thiis, jboolean bSuccess, jobject accoutslist)
{
	TArray<FString> allAccouts;
	if(!bSuccess)
	{
		AsyncTask(ENamedThreads::GameThread,[allAccouts]()
		{
			ULuaManger::Get()->OnHistoryAccountsCallBack.ExecuteIfBound(false,allAccouts);
		});
		return;
	}
	ConventJavelistToTArray(Env, accoutslist, allAccouts);
	AsyncTask(ENamedThreads::GameThread,[allAccouts]()
	{
		ULuaManger::Get()->OnHistoryAccountsCallBack.ExecuteIfBound(true,allAccouts);
	});
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_DeleteHistoryAccountCallback(JNIEnv* Env,jobject thiz, jboolean bSuccess, jlong userId)
{
	AsyncTask(ENamedThreads::GameThread,[bSuccess,userId ]()
    {
        ULuaManger::Get()->OnDelateHistoryAccountCallBack.ExecuteIfBound(bSuccess,userId);
    });
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_UpdateFileCallback(JNIEnv* Env,jobject thiz, jboolean bSuccess, jstring filepath)
{
	FString ueFilepath = FJavaHelper::FStringFromParam(Env, filepath);
	AsyncTask(ENamedThreads::GameThread,[bSuccess,ueFilepath]()
	{
		ULuaManger::Get()->OnUpdateFileCallBack.ExecuteIfBound(bSuccess,ueFilepath);
	});
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_BatteryStateChangeCallback(JNIEnv* Env,jobject thiz, jboolean status)
{
	AsyncTask(ENamedThreads::GameThread,[status]()
    {
        ULuaManger::Get()->OnBatteryChangeState.ExecuteIfBound(status);
    });
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_AgeGateCallback(JNIEnv* Env,jobject thiz, jboolean bSuccess, jlong currentTimestamp, jint  windowType, jint limitAge,jboolean isInCoolingPeriod, jstring errorMsg )
{
	FString ueStrMsg = FJavaHelper::FStringFromParam(Env, errorMsg);
	AsyncTask(ENamedThreads::GameThread,[bSuccess, currentTimestamp,windowType, limitAge, isInCoolingPeriod, ueStrMsg ]()
    {
		ULuaManger::Get()->OnAgeGateCallback.ExecuteIfBound(bSuccess,currentTimestamp, windowType, limitAge, isInCoolingPeriod, ueStrMsg);
    });
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_AntiAddicationStateChange(JNIEnv* Env,jobject thiz,  jint code,  jstring messsage )
{
	FString ueStrMsg = FJavaHelper::FStringFromParam(Env, messsage);
	AsyncTask(ENamedThreads::GameThread,[code, ueStrMsg ]()
    {
		ULuaManger::Get()->OnAntiAddictionStateChange.ExecuteIfBound(code, ueStrMsg);
    });
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_CDKeyExchangeCallback(JNIEnv* Env,jobject thiz,  jboolean bSuccess, jint code,  jstring messsage )
{
	FString ueStrMsg = FJavaHelper::FStringFromParam(Env, messsage);
	AsyncTask(ENamedThreads::GameThread,[bSuccess, code, ueStrMsg ]()
    {
        ULuaManger::Get()->OnCdKeyExchangeCallback.ExecuteIfBound(bSuccess,code, ueStrMsg);
    });
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_NetDiagnosisCallback(JNIEnv* Env,jobject thiz,  jboolean bSuccess, jint code,  jstring messsage )
{
	FString ueStrMsg = FJavaHelper::FStringFromParam(Env, messsage);
	AsyncTask(ENamedThreads::GameThread,[bSuccess, code, ueStrMsg ]()
    {
        ULuaManger::Get()->OnNetDiagnosisCallback.ExecuteIfBound(bSuccess,code, ueStrMsg);
    });
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_FetchNoticeCallback(JNIEnv* Env,jobject thiz,  jboolean bSuccess, jboolean bSaveFile,  jstring strContent,  jint errorCode)
{
	FString ueStrMsg = FJavaHelper::FStringFromParam(Env, strContent);
	AsyncTask(ENamedThreads::GameThread,[bSuccess,bSaveFile, errorCode, ueStrMsg ]()
	{
		ULuaManger::Get()->OnFetchNoticeCallback.ExecuteIfBound(bSuccess,bSaveFile, ueStrMsg, errorCode);
	});
}

JNI_METHOD void  Java_com_epicgames_unreal_NativeCallBack_OnJoinRoomResult(JNIEnv* Env,jobject thiz,  jstring roomId ,jstring userId , jint errorcode,  jboolean isRejoined)
{
	FString ueStrMsg = FJavaHelper::FStringFromParam(Env, roomId);
	FString ueStruserId = FJavaHelper::FStringFromParam(Env, userId);
	AsyncTask(ENamedThreads::GameThread,[isRejoined, errorcode, ueStrMsg,ueStruserId ]()
	{
		ULuaManger::Get()->OnJoinRomResult.ExecuteIfBound(ueStrMsg, ueStruserId,errorcode, isRejoined);
	});
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnLeaveRoom(JNIEnv* Env,jobject thiz)
{
	AsyncTask(ENamedThreads::GameThread,[ ]()
	{
		ULuaManger::Get()->OnLeaveRoom.ExecuteIfBound();
	});
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnMicrophoneEnabled(JNIEnv* Env,jobject thiz,  jstring userId , jboolean bEnable)
{
	
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnSpeakerList(JNIEnv* Env,jobject thiz, jobject speakerlist)
{
	TArray<FString> allSpeakers;
	ConventJavelistToTArray(Env, speakerlist, allSpeakers);
	AsyncTask(ENamedThreads::GameThread,[ allSpeakers]()
	{
		ULuaManger::Get()->OnSpeakerList.ExecuteIfBound(allSpeakers);
	});
	
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnSdkWebViewUrlClosed(JNIEnv* Env,jobject thiz,  jstring url)
{
	FString ueStrUrl = FJavaHelper::FStringFromParam(Env, url);
	AsyncTask(ENamedThreads::GameThread,[ueStrUrl ]()
	{
		ULuaManger::Get()->OnWebViewUrlClosed.ExecuteIfBound(ueStrUrl);
	});
}
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnSdkWebViewOpenUrlError(JNIEnv* Env,jobject thiz,  jstring url, jint errorcode,  jstring message, jint extraErrorCode,  jstring extraErrorMessage, jstring additionalInfo)
{
	FString ueStrUrl = FJavaHelper::FStringFromParam(Env, url);
	FString ueStrMessage = FJavaHelper::FStringFromParam(Env, message);
	FString ueStrExtraErrorMessage = FJavaHelper::FStringFromParam(Env, extraErrorMessage);
	FString ueStrAdditionalInfo = FJavaHelper::FStringFromParam(Env, additionalInfo);
	AsyncTask(ENamedThreads::GameThread,[ueStrUrl, errorcode,ueStrMessage,extraErrorCode,ueStrExtraErrorMessage,ueStrAdditionalInfo]()
	{
		ULuaManger::Get()->OnWebViewOpenUrlError.ExecuteIfBound(ueStrUrl, errorcode,ueStrMessage,extraErrorCode,ueStrExtraErrorMessage,ueStrAdditionalInfo);
	});
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnGetCountryCode(JNIEnv* Env,jobject thiz,   jboolean bSuccess,  jstring message, jint errorCode,  jstring strCountry,  jstring strProvince)
{
	FString ueStrInfo = FJavaHelper::FStringFromParam(Env, message);
	FString uestrCountry = FJavaHelper::FStringFromParam(Env, strCountry);
	FString uestrProvince = FJavaHelper::FStringFromParam(Env, strProvince);
	AsyncTask(ENamedThreads::GameThread,[bSuccess, ueStrInfo,errorCode,uestrCountry,uestrProvince]()
	{
		ULuaManger::Get()->OnGetCountryCodeCallback.ExecuteIfBound(bSuccess, errorCode,ueStrInfo,uestrCountry,uestrProvince);
	});
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnPayCallback(JNIEnv* Env,jobject thiz, jboolean bSuccess, jstring orderId,  jstring productId, jstring errorInfo, jint errorCode)
{
	FString ueStrOrder = FJavaHelper::FStringFromParam(Env, orderId);
	FString ueStrproductId = FJavaHelper::FStringFromParam(Env, productId);
	FString ueStrerrorInfo = FJavaHelper::FStringFromParam(Env, errorInfo);
	AsyncTask(ENamedThreads::GameThread,[bSuccess, ueStrOrder,ueStrproductId,ueStrerrorInfo,errorCode]()
	{
		ULuaManger::Get()->OnPayCallback.ExecuteIfBound(bSuccess, errorCode,ueStrOrder,ueStrproductId,ueStrerrorInfo);
	});
}
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnQueryProductInfo(JNIEnv* Env,jobject thiz, jboolean bSuccess, jstring productId,  jstring info, jint errorCode)
{
	FString ueStrproductId = FJavaHelper::FStringFromParam(Env, productId);
	FString ueStrInfo = FJavaHelper::FStringFromParam(Env, info);
	AsyncTask(ENamedThreads::GameThread,[bSuccess,ueStrproductId, ueStrInfo,errorCode]()
	{
		ULuaManger::Get()->OnQueryProductInfoCallback.ExecuteIfBound(bSuccess, errorCode,ueStrproductId,ueStrInfo);
	});
}
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnQueryGoods(JNIEnv* Env,jobject thiz, jboolean bSuccess, jstring info, jint errorCode)
{
	FString ueStrInfo = FJavaHelper::FStringFromParam(Env, info);
	AsyncTask(ENamedThreads::GameThread,[bSuccess, ueStrInfo,errorCode]()
	{
		ULuaManger::Get()->OnQueryGoodsCallback.ExecuteIfBound(bSuccess, errorCode,ueStrInfo);
	});
}
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnSetMinorLimit(JNIEnv* Env,jobject thiz,jboolean bSuccess,  jstring info, jint errorCode)
{
	FString ueStrInfo = FJavaHelper::FStringFromParam(Env, info);
	AsyncTask(ENamedThreads::GameThread,[bSuccess, ueStrInfo,errorCode]()
	{
		ULuaManger::Get()->OnSetMinorLimitCallback.ExecuteIfBound(bSuccess, errorCode,ueStrInfo);
	});
}
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnGetMinorLimit(JNIEnv* Env,jobject thiz,jboolean bSuccess,  jstring info, jint errorCode)
{
	FString ueStrInfo = FJavaHelper::FStringFromParam(Env, info);
	AsyncTask(ENamedThreads::GameThread,[bSuccess, ueStrInfo,errorCode]()
	{
		ULuaManger::Get()->OnGetMinorLimitCallback.ExecuteIfBound(bSuccess, errorCode,ueStrInfo);
	});
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnGetFriendChainPermission(JNIEnv* Env,jobject thiz,  jint nPlatformId,jboolean bOpen)
{
	AsyncTask(ENamedThreads::GameThread,[nPlatformId, bOpen]()
	{
		ULuaManger::Get()->OnGetFriendChainPermission.ExecuteIfBound(bOpen, nPlatformId);
	});
}
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnGetFriendChainInfo(JNIEnv* Env,jobject thiz,jboolean bSuccess, jint nPlatformId,  jstring info, jint errorcode)
{
	FString ueStrInfo = FJavaHelper::FStringFromParam(Env, info);
	AsyncTask(ENamedThreads::GameThread,[bSuccess, ueStrInfo,nPlatformId,errorcode ]()
	{
		ULuaManger::Get()->OnGetFriendChainInfo.ExecuteIfBound(bSuccess,nPlatformId, errorcode,ueStrInfo);
	});
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnShowExitGameDialog(JNIEnv* Env,jobject thiz)
{
	AsyncTask(ENamedThreads::GameThread,[]()
	{
		ULuaManger::Get()->OnShowExitGameDialog.ExecuteIfBound();
	});
}

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnNetMpaSetAddressCallback(JNIEnv* Env,jobject thiz, jboolean bSuccess,jstring extraMsg )
{
	FString ueStrExtraMsg = FJavaHelper::FStringFromParam(Env, extraMsg);
	AsyncTask(ENamedThreads::GameThread,[bSuccess,ueStrExtraMsg]()
	{
		ULuaManger::Get()->OnNetMpaAddAddressCallback.ExecuteIfBound(bSuccess,ueStrExtraMsg);
	});
}

#endif //PLATFORM_ANDROID