package com.epicgames.unreal;

import java.util.List;

public class NativeCallBack {
    public static final  int  _accountLogin = 1;
    public static final  int  _accountLogout = 2;
    public static final  int  _accountSwitch = 3;
    public static final  int  _accountBind = 4;
    public static final  int  _accountUnBind = 5;
    public static final  int  _accountRealNameResult = 6;
    public static final  int  _accountUserCenter = 7;
    public static native void AccountCallback(int nType, boolean bSuccess, String token, String info, String ExternData);
    public static native void HistoryAccountsCallback(boolean bSuccess, List<String> accounts);
    public static native void DeleteHistoryAccountCallback(boolean bSuccess, long userId);
    public static native void UpdateFileCallback(boolean bSuccess, String path);
    public static native void AgeGateCallback(boolean bSuccess, long currentTimestamp, int windowType, int limitAge, boolean G, String errorMsg);
    public static native void BatteryStateChangeCallback(boolean status);
    public static native void SetPlatformInfo(int platformId, String projectId, String channelId, String deviceId);
    public static native void AntiAddicationStateChange(int code, String msg);
    public static native void CDKeyExchangeCallback(boolean bSuccess,int code, String msg);
    public static native void NetDiagnosisCallback(boolean bSuccess,int code, String msg);
	public static native void FetchNoticeCallback(boolean bSuccess, boolean bSaveFile, String strContent,int errorCode);

	//RTC
    public static native void OnJoinRoomResult(String roomId,String userId,int errorCode, boolean isRejoined);
    public static native void OnLeaveRoom();
    public static native void OnMicrophoneEnabled(String userId, boolean bEnable);
    public static native void OnSpeakerList(List<String> speakers);
    //web view
    public static native void OnSdkWebViewUrlClosed(String url);
    public static native void OnSdkWebViewOpenUrlError(String url, int nCode,String message, int extraErrorCode,String extraErrorMessage,String additionalInfo);

    public static native void OnGetCountryCode(boolean bSuccess, String strCountryCode, int errorCode, String strCountry, String strProvince);
    //pay
    public static native void OnPayCallback(boolean bSuccess, String orderId, String productId,String errorInfo, int errorCode);
    public static native void OnQueryProductInfo(boolean bSuccess, String productId, String info, int errorCode);
    public static native void OnQueryGoods(boolean bSuccess,  String info, int errorCode);
    public static native void OnSetMinorLimit(boolean bSuccess,  String info, int errorCode);
    public static native void OnGetMinorLimit(boolean bSuccess,  String info, int errorCode);
    public static native void OnGetFriendChainPermission(int nPlatformId, boolean bOpen);
    public static native void OnGetFriendChainInfo(boolean bSuccess, int nPlatformId, String info, int errorCode);
    public static native void OnShowExitGameDialog();

    public static native void OnNetMpaSetAddressCallback(boolean bSuccess, String extraMsg);
}