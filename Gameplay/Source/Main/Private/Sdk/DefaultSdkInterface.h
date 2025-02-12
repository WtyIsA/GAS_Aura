
#pragma once
#include "Sdk/SdkInterface.h"

class FDefaultSdkInterface: public FSdkInterface
{
public:
	FDefaultSdkInterface():FSdkInterface(){}
	virtual  void RestartApp(UObject* obj);
	virtual  bool SdkIsInit();
	virtual  void Login();
	virtual  void InitSdk(){};

	virtual  void GetPlatformInfo();
	virtual  void ShowSdkLoginPanel(){};
	virtual	 void OpenUserCenter(){}
	virtual  void RealNameVerify(const FString& url,bool bMustSuccess) {}
	virtual  void GetAntiAddictionState(){}
	virtual  void LoginNoUi(int nType){};
	virtual  void GuestLogin(){};
	virtual  void Logout(){};
	virtual  void GetHistoryAccounts(){};
	virtual  void DeleteHistoryAccount(uint64 userid){}
	virtual  void SwitchAccount(uint64 userid){};
	virtual  FString GetLatestUserInfo(){return  FString();}
	virtual  FString GetSdkOpenId(){return FString();}
	virtual  bool IsLogin(){return false;};
	virtual  void SetDebug(bool bEnable){};
	virtual  void SetSandboxEnable(bool bEnable){};
	virtual  bool IsSandbox(){return  false;}
	virtual  bool IsEmulator(){return false;};
	virtual  double GetBatteryLevel(){return 1.0f;};
	virtual  bool MobileIsCharging(){return false;}
	virtual  bool CanAutoLogin(){return  false;};
	virtual  void AutoLogin(){}
	virtual  void AccountBind(int nLoginType){}
	virtual  void AccountUnBind(int nLoginType){}
	virtual  void AccountForceRebend(int nAccountType, int force){}
	virtual  bool CheckProtocolVersion(){return false;}
	virtual  void SetUserHasAgreedProtocolsUpdate(){}
	virtual  void Report(const FString& eventName, const FString& key, const FString& value, const FString& extraData){}
	virtual  void Report(const FString& eventName, const FString& jsonContent){}
	virtual  void UploadFile(const FString& filePath,uint64 roleId,const FString& roleName){}
	virtual	 void UploadException(const FString&  name, const FString&  reason, const FString&  stackTrace){}
	virtual  void JudgeAgeGate(){};
	virtual  void ModifyAgeGateStatus(bool bEnable){};
	virtual  bool QueryAgeGateStatus(){return true;};
	virtual  void GetUserAgreementUrlAndPrivacyPolicy(FString& strUserAgreementUrl,FString& strPrivacyPolicy){}
	virtual  int  GetCurrentNetState() { return 1;}
	virtual  void VerifyActivationCode(const FString& strCode){};
	virtual  void OpenUrl(const FString& title, const FString& url, const FString& extraData){}
	virtual  void CDKeyExchange(const FString& cdkey, const FString& roleid, const FString& serverid){};
	virtual  void NetDiagnosis(const  FString& target, int timeout){}
	virtual  void AddLocalNotification(const FString& title,const FString& content, uint64 nDelaySeconds){};
	virtual  void FetchNotice(int nScene, const FString& savePath, const FString& strLanguage, const FString& otherParams){};
	virtual  void LogSceneStart(const FString& sceneName){}
	virtual  void LogSceneLoaded(){}
	virtual  void LogSceneEnd(){}
	virtual  void SetUserInfoForSecureService(const FString& roleId,const FString& serverId){}
	virtual  bool IsPermissionGranted(const FString& strPermission){return true;};
	virtual  void OpenAppSetting(){};
	virtual  bool RequestPermission(const FString& strPermission, bool bFirstRequest){return true;}
	virtual  bool IsNotificationsAllowed(const FString& strChannelName){return true;}
	virtual  void GetCountryCode();
	virtual  void ChangeLanguage(const FString& langType){}
	virtual  void ShowRatingUi(){}
	virtual  bool IsBsdkPay(){return true;};
	virtual  void Pay(const FString& productId, int goodType, int amount, const FString& currency,
		const FString& roleId,const FString&roleName, int roleLevel, int roleVipLevel,
		const FString& serverId,const FString& extInfo){};
	virtual  void QueryProducts(const FString& productId){}; //查询商品价格信息
	virtual  void QueryGoods(const FString& roleId,const FString& serverId, bool bCurrency){}
	virtual  void SetMinorLimit(const FString& country,const FString& content) {}
	virtual  void GetMinorLimit(const FString& country) {}
	virtual  void HasOpenFriendChainPermission(int32  platformId);
	virtual  void GetFriendChainInfo(int32 platformId);
	virtual  void ShowRatingCustomUi(){}
	virtual  void DismissRatingCustomUi(){}
	virtual  void OnRatingPraiseBtnClick(){}
	virtual  void OnRatingFeedbackBtnClick(){}
	virtual  FString GetChannelOp(){return  TEXT("bsdk");}
	virtual  void ChannelGeneralAction(int32 code){}
	virtual  bool UnZip(const FString& filePath,const FString& destDirPath, bool bSoFile){return true;}

	virtual  void NetMpaStart(const FString& startLog) {}
	virtual  bool NetMpaAddAddress(const TArray<FString>& address,const FString& contextId);
	virtual  void NetMpaStop(const FString& stopLog) {}

	virtual  void AddUploadLogfileFullPaths(const TArray<FString>& files) {}
	virtual  void AddUploadLogfileDirPath(const FString& fileDir) {}
};