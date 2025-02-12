#pragma once

class MAIN_API FSdkInterface
{
protected:
	FSdkInterface(){}	
public:
	virtual ~FSdkInterface(){}
	static FSdkInterface* GetIns();
	static void SetSdkInterface(FSdkInterface* pIns);

	virtual  void RestartApp(UObject* obj) = 0;
	
	virtual  bool SdkIsInit() = 0;
	virtual  void InitSdk() = 0;
	virtual  void Login() = 0;

	virtual  void GetPlatformInfo() = 0;
	virtual  bool CanAutoLogin() =0;
	virtual  void AutoLogin() =0;
	virtual  void ShowSdkLoginPanel() =0;
	virtual	 void OpenUserCenter()=0;
	virtual  void RealNameVerify(const FString& url,bool bMustSuccess)= 0;
	virtual  void GetAntiAddictionState() = 0;
	virtual  void AccountBind(int nLoginType) =0;
	virtual  void AccountUnBind(int nLoginType) =0;
	virtual  void AccountForceRebend(int nAccountType, int force) =0;
	virtual  bool CheckProtocolVersion() =0;
	virtual  void SetUserHasAgreedProtocolsUpdate() = 0;
	virtual  void Report(const FString& eventName, const FString& key, const FString& value, const FString& extraData) = 0;
	virtual  void Report(const FString& eventName, const FString& jsonContent) = 0;
	virtual  void LoginNoUi(int nType) = 0;
	virtual  void GuestLogin() = 0;
	virtual  void Logout() = 0;
	virtual  void GetHistoryAccounts() = 0;
	virtual  void DeleteHistoryAccount(uint64 userid) = 0;
	virtual  void SwitchAccount(uint64 userid) = 0;
	virtual  FString GetLatestUserInfo() = 0;
	virtual  bool IsLogin() = 0;
	virtual  void SetDebug(bool bEnable) = 0;
	virtual  void SetSandboxEnable(bool bEnable) = 0;
	virtual  bool IsSandbox() = 0;
	virtual  bool IsEmulator() = 0;
	virtual  double GetBatteryLevel() = 0;
	virtual  bool MobileIsCharging() = 0; //手机是否在充电
	virtual  FString GetSdkOpenId() = 0;
	virtual  void GetUserAgreementUrlAndPrivacyPolicy(FString& strUserAgreementUrl,FString& strPrivacyPolicy) = 0;

	virtual	 void UploadFile(const FString& filePath,uint64 roleId,const FString& roleName) = 0;
	virtual	 void UploadException(const FString&  name, const FString&  reason, const FString&  stackTrace) = 0;
	virtual  void JudgeAgeGate()=0;
	virtual  void ModifyAgeGateStatus(bool bEnable) = 0;
	virtual  bool QueryAgeGateStatus() = 0;
	virtual  int  GetCurrentNetState() = 0;
	virtual  void VerifyActivationCode(const FString& strCode) = 0;
	virtual  void OpenUrl(const FString& title, const FString& url, const FString& extraData) = 0;
	virtual  void CDKeyExchange(const FString& cdkey, const FString& roleid, const FString& serverid) = 0;
	virtual  void NetDiagnosis(const  FString& target, int timeout) = 0;
	virtual  void AddLocalNotification(const FString& title,const FString& content, uint64 nDelaySeconds) = 0;
	virtual  void FetchNotice(int nScene, const FString& savePath, const FString& strLanguage, const FString& otherParams) = 0;

	virtual  void LogSceneStart(const FString& sceneName) = 0;
	virtual  void LogSceneLoaded() = 0;
	virtual  void LogSceneEnd() = 0;

	virtual  void SetUserInfoForSecureService(const FString& roleId,const FString& serverId) = 0;
	virtual  bool IsPermissionGranted(const FString& strPermission) = 0;
	virtual  void OpenAppSetting() = 0;
	virtual  bool RequestPermission(const FString& strPermission, bool bFirstRequest) = 0;
	virtual  bool IsNotificationsAllowed(const FString& strChannelName) = 0;
	virtual  void GetCountryCode() = 0;
	virtual  void ChangeLanguage(const FString& langType) = 0;
	virtual  void ShowRatingUi() = 0;
	virtual  void ShowRatingCustomUi() = 0;
	virtual  void DismissRatingCustomUi() = 0;
	virtual  void OnRatingPraiseBtnClick() = 0;
	virtual  void OnRatingFeedbackBtnClick() = 0;

	//pay
	virtual  bool IsBsdkPay() = 0;
	virtual  void Pay(const FString& productId, int goodType, int amount, const FString& currency,
		const FString& roleId,const FString&roleName, int roleLevel, int roleVipLevel,
		const FString& serverId,const FString& extInfo) = 0;
	virtual  void QueryProducts(const FString& productId) = 0; //查询商品价格信息
	virtual  void QueryGoods(const FString& roleId,const FString& serverId, bool bCurrency) = 0;
	virtual  void SetMinorLimit(const FString& country,const FString& content) = 0;
	virtual  void GetMinorLimit(const FString& country) = 0;
	virtual  void HasOpenFriendChainPermission(int32  platformId) = 0;
	virtual  void GetFriendChainInfo(int32 platformId) = 0;
	virtual  FString GetChannelOp() = 0;
    virtual  void ChannelGeneralAction(int32 code) = 0;
	virtual  bool UnZip(const FString& filePath,const FString& destDirPath, bool bSoFile) = 0;

	virtual  void NetMpaStop(const FString& stopLog) = 0;
	virtual  bool NetMpaAddAddress(const TArray<FString>& address,const FString& contextId) = 0;
	virtual  void NetMpaStart(const FString& startLog) = 0;
	virtual  void AddUploadLogfileFullPaths(const TArray<FString>& files) = 0;
	virtual  void AddUploadLogfileDirPath(const FString& fileDir) = 0;

	virtual  void GetFriends(){}
	virtual  void InviteFriend(const FString& friendID, const FString& inviteInfo){}
	virtual  void SendMsgToFriend(const FString& friendID, const FString& msg){}
private:
	static FSdkInterface* m_SdkInterface;
};