


#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_SetPlatformInfo(JNIEnv* Env,jobject thiz, jint platformId, jstring projectId, jstring channelId, jstring appChannelId);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_AccountCallback(JNIEnv* Env,jobject thiz, jint nType,jboolean bSuccess, jstring strToken, jstring strData, jstring strExtenData);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_HistoryAccountsCallback(JNIEnv* Env,jobject thiis, jboolean bSuccess, jobject accoutslist);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_DeleteHistoryAccountCallback(JNIEnv* Env,jobject thiz, jboolean bSuccess, jlong userId);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_UpdateFileCallback(JNIEnv* Env,jobject thiz, jboolean bSuccess, jstring path);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_BatteryStateChangeCallback(JNIEnv* Env,jobject thiz, jboolean status);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_AgeGateCallback(JNIEnv* Env,jobject thiz, jboolean bSuccess, jlong currentTimestamp, jint  windowType, jint limitAge,jboolean isInCoolingPeriod, jstring errorMsg );
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_AntiAddicationStateChange(JNIEnv* Env,jobject thiz,  jint code,  jstring messsage );
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_CDKeyExchangeCallback(JNIEnv* Env,jobject thiz,  jboolean bSuccess, jint code,  jstring messsage );
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_NetDiagnosisCallback(JNIEnv* Env,jobject thiz,  jboolean bSuccess, jint code,  jstring messsage );
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_FetchNoticeCallback(JNIEnv* Env,jobject thiz,  jboolean bSuccess, jboolean bSaveFile,  jstring messsage,  jint errorCode);


//RTC
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnJoinRoomResult(JNIEnv* Env,jobject thiz,  jstring roomId , jstring userId ,  jint errorcode,  jboolean isRejoined);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnLeaveRoom(JNIEnv* Env,jobject thiz);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnMicrophoneEnabled(JNIEnv* Env,jobject thiz,  jstring userId , jboolean bEnable);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnSpeakerList(JNIEnv* Env,jobject thiz,  jobject speakerlist);
//web view
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnSdkWebViewUrlClosed(JNIEnv* Env,jobject thiz,  jstring url);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnSdkWebViewOpenUrlError(JNIEnv* Env,jobject thiz,  jstring url, jint errorcode,  jstring message, jint extraErrorCode,  jstring extraErrorMessage, jstring additionalInfo);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnGetCountryCode(JNIEnv* Env,jobject thiz,   jboolean bSuccess,  jstring message, jint errorCode,  jstring strCountry,  jstring strProvince);
//pay
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnPayCallback(JNIEnv* Env,jobject thiz, jboolean bSuccess, jstring orderId,  jstring productId, jstring errorInfo, jint errorcode);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnQueryProductInfo(JNIEnv* Env,jobject thiz, jboolean bSuccess, jstring productId,  jstring info, jint errorcode);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnQueryGoods(JNIEnv* Env,jobject thiz, jboolean bSuccess, jstring info, jint errorcode);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnSetMinorLimit(JNIEnv* Env,jobject thiz,jboolean bSuccess,  jstring info, jint errorcode);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnGetMinorLimit(JNIEnv* Env,jobject thiz,jboolean bSuccess,  jstring info, jint errorcode);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnGetFriendChainPermission(JNIEnv* Env,jobject thiz,  jint nPlatformId,jboolean bOpen);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnGetFriendChainInfo(JNIEnv* Env,jobject thiz,jboolean bSuccess, jint nPlatformId,  jstring info, jint errorcode);
JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnShowExitGameDialog(JNIEnv* Env,jobject thiz);

JNI_METHOD void Java_com_epicgames_unreal_NativeCallBack_OnNetMpaSetAddressCallback(JNIEnv* Env,jobject thiz, jboolean bSuccess,jstring extraMsg );
#endif