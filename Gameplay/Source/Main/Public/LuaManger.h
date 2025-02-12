#pragma once
#include "CoreMinimal.h"
#include "CPathVolume.h"

#include "UObject/NoExportTypes.h"
#include "UnLuaInterface.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/Image.h"
#include "Components/ListView.h"
#include "Engine/WorldComposition.h"
#include "GameplayEffect.h"
#include "GameplayAbilities/Public/Abilities/GameplayAbilityTypes.h"
#include "Input/CommonUIInputSettings.h"
#include "LuaManger.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(GAME, Log, Display)
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLuaResLoadDelegate, const FString&, ResPath, UObject*, ResObject);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FResLoadDelegate, const FString&, ResPath, UObject*, ResObject);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FMutliResLoadDelegate, TArray<FString>, ResPathList, TArray<UObject*>, ResList);
DECLARE_DYNAMIC_DELEGATE_FourParams(FSdkLoginDelegate, bool, bSuccess,const FString&, strToken,const FString&, strData,const FString&, strExtenData);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FSdkAccountsDelegate, bool, bSuccess,const TArray<FString>&, accountList);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FSdkDelegate, bool, bSuccess,const FString&, filePath);
DECLARE_DYNAMIC_DELEGATE_OneParam(FSdkBoolDelegate, bool, bState);
DECLARE_DYNAMIC_DELEGATE_OneParam(FSdkStringDelegate, const FString&, message);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FSdkBoolLongDelegate, bool, bSuccess,uint64, userId);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FSdkLongStringDelegate,uint64, code, const FString&, message);
DECLARE_DYNAMIC_DELEGATE_SixParams(FSdkAgeDelegate, bool, bSuccess, uint64, timestamp, int32, windowTypeOrErrorCode, int32, limitage, bool, isInCoolingPeriod,const FString&, errorMsg);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FSdkBoolIntStringDelegate, bool, bSuccess, int32, code,const FString&, strMsg);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWindowActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWindowDeActive);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnActorRelevantChange, AActor*,Actor, bool,bRelevant);
DECLARE_DYNAMIC_DELEGATE_FiveParams(FOnHttpDownloadComplete, const FString& ,strUrl, bool, bSuccess, const FString&,contentOrsavePath, bool, bSaveFile, int, errorCode);
DECLARE_DYNAMIC_DELEGATE_FourParams(FSdkFetchNoticeDelegate, bool, bSuccess,bool, bSaveFile,const FString&, strData,int32, errorCode);
DECLARE_DYNAMIC_DELEGATE_FourParams(FSdkBoolIntTwoStringsDelegate, bool, bSuccess,int, errorCode,const FString&, strData,const FString&, strExtenData);
DECLARE_DYNAMIC_DELEGATE_FourParams(FSdkBoolTwoIntStringDelegate, bool, bSuccess,int, param1, int, param2, const FString&, strData);
DECLARE_DYNAMIC_DELEGATE_FiveParams(FSdkBoolIntThreeStringsDelegate, bool, bSuccess,int, errorCode,const FString&, strDta1,const FString&, strData2,const FString&, strData3);

DECLARE_DYNAMIC_DELEGATE_FourParams(FOnVoiceJoinRoomResult, const FString&, roomId, const FString&, userId, int, errorCode, bool, isRejoined);
DECLARE_DYNAMIC_DELEGATE(FOnVoiceLeaveRoom);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnVoiceMicrophoneEnabled, const FString&, userId, bool, isRejoined);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnVoiceSpeakerList,const TArray<FString>&, speaks);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnSpeakerVolumeChanged,const FString&, roomId,const FString&, userId,int, nVolume);
DECLARE_DYNAMIC_DELEGATE_SixParams(FOnOpenUrlError,const FString& ,strUrl,  int32, code,const FString& ,strMessage,  int32, extraErrorCode,const FString& ,strExtraErrorMessage,  const FString&, strAdditionalInfo);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnReplicateActorFail, APlayerController* ,AActor* );
DECLARE_DYNAMIC_DELEGATE(FSdkVoidDelegate);
class UWidget;
class UDragDropWidget;
class UFPSManager;
class FLuaGcTickable;
class USessionMgr;
class ReplicationGraph;
struct Table;

UCLASS()
class ULuaManger: public UObject, public IUnLuaInterface
{
    GENERATED_BODY()
    void UnInit();
public:
    static ULuaManger* Get(UGameInstance* pGameIns=NULL);
    static void Free();
    virtual FString GetModuleName_Implementation() const  {

        if(IsRunningDedicatedServer())
        {
            return TEXT("Server.LuaMgr");
        }
        else
        {
            return TEXT("Client.LuaMgr");
        }
        
    }

    void Tick();
    UFUNCTION()
    void SetTimer(float DeltaTime);
    UFUNCTION()
    void SetLuaSearchPathAndOcPath(const TArray<FString>& luaSearchPaths, const FString& ocPath);

    UFUNCTION(BlueprintNativeEvent)
    void TickInLua(float DeltaSeconds);

    void PreInit();
    UFUNCTION(BlueprintNativeEvent)
    void Init();
    void PostInit();
    UFUNCTION(BlueprintNativeEvent)
    void OnPersistantLevelLoaded();

    UFUNCTION(BlueprintCallable)
    UFPSManager* GetFPSManager();
    UFUNCTION(BlueprintCallable)
    UAttackTracerMgr* GetAttackTracerManager();
    UFUNCTION(BlueprintCallable)
    void SetEnableMotionData(bool enable);

    UFUNCTION()
    FString GetLuaSrcPath(FString& luaScriptName)const;
    UFUNCTION()
    FString GetPersistentDownloadDir()const;
    UFUNCTION()
    FString GetPakSavePath() const;
    UFUNCTION()
    UObject* GetResMgr();
    
    UFUNCTION(BlueprintNativeEvent)
    void Start();
    
    UFUNCTION(BlueprintNativeEvent)
    void Shutdown();
    
    static ULuaManger*    handle;

    UFUNCTION()
    void EmptyOverrideMaterials(UMeshComponent* mesh);
    UFUNCTION()
    FString GetMaterialPath(UMaterialInstance* mi);
    UFUNCTION()
    void MarkObjectToDestory(UObject* pObject);
    UFUNCTION()
    void AddUObjectToRoot(UObject* pObject);
    UFUNCTION()
    void RemoveUObjectFromRoot(UObject* pObject);
    UFUNCTION(BlueprintImplementableEvent)
     void LuaFree();
    UFUNCTION()
    void ToGc();
    UFUNCTION()
    UGameUserSettings* GetGameUserSettings();
    UFUNCTION()
    void SetScalabilityLevel(uint8 Scalability);
    UFUNCTION()
    void SetShadowQuality(uint8 quality);
    UFUNCTION()
    void SetMaterialQualityLevel(uint8 quality);
    UFUNCTION()
    void SetMSAA(uint8 quality);
    UFUNCTION()
    uint32 GetGBRam();
    UFUNCTION()
    uint64 GetMRam();
    UFUNCTION()
    uint64 GetAvailableMRam();
    UFUNCTION()
    FString GetPerformanceLevel();
    UFUNCTION()
    FString GetActiveDeviceProfileName();
    UFUNCTION()
    void SetUROParam(USkeletalMeshComponent* mesh,TArray<float>& thresholds);

    UFUNCTION()
    void IncreaseBodyInstanceDamping(USkeletalMeshComponent* TargetMesh, float MultiFactor, FName InRootBoneName = TEXT("root"));
    UFUNCTION()
    void ResetBodyInstanceDamping(USkeletalMeshComponent* TargetMesh);

    UFUNCTION()
    FString GetMachineId();
    UFUNCTION()
    FString GetOperatingSystemId();
    UFUNCTION()
    FString GetMacAddressString();
    
    //UUserWidget
    #pragma region UUserWidget
    
    UFUNCTION()
    void AddWidgetToGameViewPort(UUserWidget* widget, int nOrder);
    UFUNCTION()
    void RemoveWidgetFromGameViewPort(UUserWidget* widget);
    UFUNCTION()
    void SetInputMode(bool bUiOnly, bool bShowMouse, UWidget* widget, bool bLockMouse);
    UFUNCTION()
    int32 GetStringAsciiLen(FString str);
    UFUNCTION()
    FString CheckStringAsciiLen(FString str, int nLen);
    UFUNCTION(BlueprintNativeEvent)
    void DragWidget(UObject* pDragObj,int32 nDragType,int32 nDropType, UWidget* pDrop, UUserWidget* pTopParent);
    UFUNCTION(BlueprintNativeEvent)
    void OnBeginDragWidget(UObject* pDragObj,int32 nDragType, UUserWidget* pTopParent, UDragDropWidget* _CurDragVisualWidget);
    UFUNCTION(BlueprintNativeEvent)
    void OnCancelDragWidget(UObject* pDragObj,int32 nDragType, UUserWidget* pTopParent);
    UFUNCTION()
    UWidget* GetTopWidget(UWidget* pCtrl);
    UFUNCTION()
    FVector2D GetWidgetSize(UWidget* pWidget);
    UFUNCTION()
    FVector2D GetWidgetAbsolutePostion(UWidget* pWidget);
    UFUNCTION()
    bool IsPostionUnderWidget(UWidget* pWidget, FVector2D AbsolutePos);
    UFUNCTION()
    FVector2D GetViewportSize();
    UFUNCTION()
    void GetAllUText(UUserWidget* pWidget, TArray<UWidget*>& allText);
    UFUNCTION()
    void GetWidgetTexts(UWidget* pWidget,  TArray<UWidget*>& allText);
    UFUNCTION()
    void GetAllImageGlobal(UUserWidget* pWidget, TArray<UWidget*>& allImage);
    UFUNCTION(BlueprintNativeEvent)
    bool OnWigetMouseButtonDown(UWidget* topwidget,UUserWidget* widget, FVector2D screenPos);
    UFUNCTION(BlueprintNativeEvent)
    bool OnWigetRightMouseButtonDown(UWidget* topwidget,UUserWidget* widget, FVector2D screenPos);
    UFUNCTION(BlueprintNativeEvent)
    bool OnWigetMouseButtonUp(UWidget* topwidget,UUserWidget* widget, FVector2D screenPos);
    UFUNCTION(BlueprintNativeEvent)
    bool OnWigetMouseButtonMove(UWidget* topwidget,UUserWidget* widget,FVector2D screenPos, FVector2D CursorDelta);
    UFUNCTION(BlueprintNativeEvent)
    bool OnWigetMouseLeave(UWidget* topwidget,UUserWidget* widget);
    UFUNCTION()
    bool CheckKeyDown(FString keyName);
    UFUNCTION()
    FVector2D GetWidgetRelativePos(FVector2D pos, UWidget* selfWidget, UWidget* parentWidget);
    UFUNCTION()
    void SetListAlignment(UWidget* pWidget, int alignment);
    UFUNCTION()
    UListViewBase* GetListViewFromListEntry(UUserWidget* listEntry);
#pragma endregion
    
#pragma region file  
    UFUNCTION()
    void WriteIniFile(const FString& strSection, const FString& key, const FString& value);
    UFUNCTION()
    FString GetIniFile(const FString& strSection, const FString& key, const FString& Name);
    UFUNCTION()
    void EmptySectionIniFile(const FString& strSection, bool bMatchKey = false);
    UFUNCTION()
    void RemoveKeyIniFile(const FString&  strSection, const FString&  key);
    UFUNCTION()
    FString GetFileContent(FString strFilePath, bool bProjectFile = true);
    UFUNCTION()
    bool SaveStringToFile(const FString& strFilePath, const FString& strContent);
    UFUNCTION()
    void CreateDir(FString strPath);
    UFUNCTION()
    void DeleteDir(FString strPath);
    UFUNCTION()
    void MountPaks();
    UFUNCTION()
    bool FileIsExist(FString filePath, bool bProjectRelativeFile);
    UFUNCTION()
    void GetFilesFromPath(const FString& filePath,const FString& FileExtension, TArray<FString>& files);
    UFUNCTION()
    bool DeletePak(const FString& pakName);
    UFUNCTION()
    int32 UnMountSavePaks(bool bAutoDelete);
#pragma endregion
    //快捷键
    #pragma region Shortcut
    UFUNCTION()
    void AddShortcutKey(FString keyName, UObject* obj = nullptr, bool bShift = false, bool bAlt = false,bool bCtrl = false, bool bCmd = false, int32 inputEvent = 0);
    UFUNCTION()
    void RemoveShortcutKey(FString keyName, UObject* obj = nullptr, bool bShift = false, bool bAlt = false,bool bCtrl = false, bool bCmd = false, int32 inputEvent =0 );
    UFUNCTION()
    void RemoveObjectShortcutKeys(UObject*obj);
    UFUNCTION(BlueprintNativeEvent)
    void OnShortcutKey(UObject* widget, const FString& keyName,bool bShift, bool bAlt,bool bCtrl, bool bCmd, int32 inputEvent);
    UFUNCTION()
    UInputComponent* GetInputComponent();
    UFUNCTION()
    void AddAxisKey(FString keyName, UObject* obj = nullptr);
    UFUNCTION()
    void RemoveAxisKey(FString keyName, UObject* obj = nullptr);
    UFUNCTION(BlueprintNativeEvent)
    void OnAxisKey(UObject* widget, const FString& keyName,float axisvalue, FVector2D touch1, FVector2D touch2);
    UFUNCTION()
    bool ChangeActionKey(FString actionName, FString fromkeyName, FString tokeyName, bool bShift, bool bAlt,bool bCtrl, bool bCmd);
    UFUNCTION()
    bool ChangeAxisKey(FString actionName, FString fromkeyName, FString tokeyName);
    UFUNCTION()
    void ForceRebuildKeymaps();
    UFUNCTION()
    bool ChangeShortcutKey(FString fromKeyName, FString toKeyname, UObject* obj = nullptr, bool bShift = false, bool bAlt = false,bool bCtrl = false, bool bCmd = false, int32 inputEvent = 0);

    UFUNCTION()
    void RegisterSettingInputProcess();
    UFUNCTION()
    void UnregisterSettingInputProcess();
    UFUNCTION(BlueprintImplementableEvent)
    void OnPressKey(const FString& KeyName);

    TSharedPtr<IInputProcessor> SettingInputProcessor;
    
#pragma endregion 

#pragma region level
    UFUNCTION()
    void OpenLevel(FString levelName);
    UFUNCTION()
    void LoadStreamLevel(FString levelName,bool bMakeVisibleAfterLoad, bool bShouldBlockOnLoad);
    UFUNCTION()
    void UnLoadStreamLevel(FString levelName, bool bShouldBlockOnLoad);
    UFUNCTION()
    void LoadWorldStreamLevel(UObject* worldObject, FString levelName,bool bMakeVisibleAfterLoad, bool bShouldBlockOnLoad);
    UFUNCTION()
    void UnLoadWorldStreamLevel(UObject* worldObject,FString levelName, bool bShouldBlockOnLoad);
    UFUNCTION()
    bool LoadMap(FName WorldContextHandle, const FString& mapPath);
    UFUNCTION(BlueprintNativeEvent)
    void OnLoadStreamLevel(  const FString& worldName, UWorld* pWorld);
    UFUNCTION(BlueprintNativeEvent)
    void OnUnLoadStreamLevel(  const FString& worldName, UWorld* pWorld);
    UFUNCTION(BlueprintNativeEvent)
    void OnLevelLoaded(UWorld* pWorld,const FString& levelName);
    UFUNCTION(BlueprintNativeEvent)
    void OnDsNetWorkFailure(UWorld * World, const FString& worldName, UNetDriver *NetDriver, int32 nailureType, const FString& ErrorString);
    UFUNCTION(BlueprintNativeEvent)
    void OnLevelUnloadComplete(UWorld * World, const FString& worldName, bool bSessionEnded, bool bCleanupResources);
    
    UFUNCTION(BlueprintCallable, Category="LuaMgr|World", meta = (Latent, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
    bool ChangePrecomputedVisibilityHandler(UObject* WorldContextObject, const FString& InLongPackageName);
	
    UFUNCTION(BlueprintCallable, Category="LuaMgr|World", meta = (Latent, HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
    const ULevelStreaming* GetLevelByPackageName (UObject* WorldContextObject, const FString& InLongPackageName);
    
    UFUNCTION()
    bool AllSubLevelIsLoaded();
    UFUNCTION()
    bool IsWorldReady(const FName& WorldContextHandle);
    UFUNCTION()
    bool CheckLevelStreamingDistanceVisible();
    UFUNCTION()
    bool IsServerWorldReady(UWorld* World);
    UFUNCTION()
    static void AddActorToLevelCluster(AActor* Actor);
    UFUNCTION()
    void AddObjectToActorCluster(UObject* Object, AActor* Actor);
    UFUNCTION()
    void AddToAlwaysRelevant(AActor* Actor,APlayerController* PlayerController);
    UFUNCTION()
    void RemoveFromAlwaysRelevant(AActor* Actor, APlayerController* PlayerController);
#pragma endregion

#pragma region http
    UFUNCTION()
    FString EncodeUrl(FString url);
    UFUNCTION()
    void StartDownload(FString url, FString savepath, int32 nRetryTimes = 5, int64 nTotalSize = 0, int32 nThreadCount=1, FString strMd5=TEXT(""),const FOnHttpDownloadComplete& callback=FOnHttpDownloadComplete());
    UFUNCTION()
    void StartDownloadRange(FString url, FString savepath,TArray<int32> nRanges, int32 nRetryTimes = 5,  FString strMd5=TEXT(""),const FOnHttpDownloadComplete& callback=FOnHttpDownloadComplete());
    UFUNCTION()
    void StartDownloadLargeFileRange(FString url, FString savepath,TArray<FString> nRanges, int32 nRetryTimes = 5,  FString strMd5=TEXT(""),const FOnHttpDownloadComplete& callback=FOnHttpDownloadComplete());
    UFUNCTION()
    void StopDownload(FString url);
    UFUNCTION()
    void PauseDownload(const FString& url);
    UFUNCTION()
    void ContinueDownload(const FString& url);
    UFUNCTION()
    bool HttpIsDownloading(const FString& url);
    UFUNCTION(BlueprintNativeEvent)
    void OnDownloadComplete(const FString& strUrl, bool bSuccess, const FString&contentOrsavePath, bool bSaveFile, int errorCode = 0);
    UFUNCTION()
    void HttpDataWriteToDisk();
    UFUNCTION()
    int64 GetHttpDownloadedLen(FString& downloadInfo);
    UFUNCTION()
    int64 GetTaskDownloadedSize(const FString& strUrl);
    UFUNCTION()
    void ReDownloadErrorTask();
    UFUNCTION()
    void OpenUrl(FString strUrl);
#pragma endregion

    UFUNCTION()
    int64 GetTick();
    UFUNCTION()
    int32 GetLuaMem();
    UFUNCTION()
    void Copy(FString& str);
#pragma region server
    UFUNCTION()
    UBTNode* GetActiveNodeFromBehaviorTree(UBehaviorTreeComponent* pTree);
    UFUNCTION()
    int32 GetActiveInstanceIdFromBehaviorTree(UBehaviorTreeComponent* pTree);
#pragma endregion

#pragma region ForkRelated
    UFUNCTION()
    bool DirectoryExists(FString dirPath);

    UFUNCTION()
    FString ProjectContentDir();

    UFUNCTION()
    void WaitAndFork();

    UFUNCTION()
    bool IsForkedChildProcess();

    UFUNCTION()
    FString GetCommandLine();

    UFUNCTION()
    void SetCommandLine(FString Command);

    UFUNCTION()
    FString GetMapName();

    UFUNCTION()
    uint32 GetProcessId();

    UFUNCTION()
    bool IsLinuxPlatform();

    UFUNCTION()
    uint16 GetForkedChildProcessIndex();
    
    UFUNCTION()
    uint32 GetListenPort();

    UFUNCTION()
    void StartListen(int Port);

    bool IsRegisteredRelevantActor(AActor * Actor);

    UPROPERTY()
    TSet<UClass*> NeedNotifyClasses;

    void OnActorRelevantChange(AActor* Actor,bool IsRelevant);

    UFUNCTION()
    void CreateProc(FString URL, FString Params, bool bLaunchDetached, bool bLaunchHidden, bool bLaunchReallyHidden);
#pragma endregion

#pragma region Sdk
    UFUNCTION()
    bool SdkIsInit();
    UFUNCTION()
    void RestartApp();
    UFUNCTION()
    void GetPlatformInfo();
    UFUNCTION(BlueprintNativeEvent)
    void SetPlatformInfo(int nPlatformId, const FString& ProjectId, const FString& channelId, const FString& deviceId);
    UFUNCTION()
    void ShowLoginPanel();
    UFUNCTION()
    void OpenUserCenter();
    /*
     * VISITOR = 1;GOOGLE = 5;FACEBOOK = 6; LINE = 8; KAKAO = 9; VK = 10;TIKTOK=15
     * GUEST = 1000;
     */
    UFUNCTION()
    bool CanAutoLogin();
    UFUNCTION()
    void AutoLogin();
    UFUNCTION()
    void Login(int nLoginType);
    UFUNCTION()
    void AccountBind(int nLoginType,const FSdkLoginDelegate& OnBindAccoutBack);
    UFUNCTION()
    void AccountUnBind(int nLoginType,const FSdkLoginDelegate& OnUnBindAccoutBack);
    /*
    force 0 ：取消， 1：强制绑定 
    */
    UFUNCTION()
    void AccountForceRebend(int nAccountType, int force,const FSdkLoginDelegate& OnBindAccoutBack);
    UFUNCTION()
    FString GetLoginUserInfo();
    UFUNCTION()
    void LogOut();
    UFUNCTION()
    void SwitchLogin(uint64 userId,const FSdkLoginDelegate& OnSwitchAccoutBack);
    UFUNCTION()
    void GetHistoryAccounts(const FSdkAccountsDelegate& OnHistoryAccout);
    UFUNCTION()
    void DeleteHistoryAccount(uint64 userId, const FSdkBoolLongDelegate& OnDeleteResult);
    UFUNCTION()
    bool IsEmulator();
    UFUNCTION()
    bool IsSandbox();
    UFUNCTION()
    void SetSandbox(bool bEnable);
    UFUNCTION()
    void SetDebug(bool bEnable);
    UFUNCTION()
    double GetBatteryLevel();
    UFUNCTION()
    bool MobileIsCharging();
    UFUNCTION()
    bool CheckProtocolVersion();
    UFUNCTION()
    void SetUserHasAgreedProtocolsUpdate();
    UFUNCTION()
    void GetUserAgreementUrlAndPrivacyPolicy(FString& strUserAgreementUrl,FString& strPrivacyPolicy);
    /*埋点*/
    UFUNCTION()
    void Report(const FString& eventName,const FString& key, const FString& value, const FString& extraData);
    /*
     * 上传异常日志到SDK后台
     */
    UFUNCTION()
    void UploadException(const FString&  name, const FString&  reason, const FString&  stackTrace);
    /*
     * 上传日志文件到SDK后台
     * callback :  bSuccess:bool, filePath:string
     */
    UFUNCTION()
    void UploadLog(const FString& filePath,uint64 roleId,const FString& roleName, const FSdkDelegate& updateCallback);
    UFUNCTION()
    void JudgeAgeGate(const FSdkAgeDelegate& ageCallback);
    UFUNCTION()
    void ModifyAgeGateStatus(bool bEnable);
    UFUNCTION()
    bool QueryAgeGateStatus();
    UFUNCTION()
    FString GetSdkOpenId();
    UFUNCTION()
    void AddLocalNotification(const FString& title,const FString& content, uint64 nDelaySeconds);
    /*
    * 获取当前设备网络状态 WIFI 2g-5g
    * 返回值 -1:未知 0:未连接 1:WIFI 2:2G 3:3G 4:4G 5:5G
    */
    UFUNCTION()
    int32 GetCurrentNetState();
    UFUNCTION()
    void VerifyActivationCode(const FString& strCode);
    UFUNCTION()
    void OpenUrlBySdk(const FString& title, const FString& url, const FString& extraData);
    UFUNCTION()
    void RealNameVerify(const FString& url,bool bMustSuccess,const FSdkLoginDelegate& realNameCallback);
    UFUNCTION()
    void GetAntiAddictionState();
    UFUNCTION()
    void CDKeyExchange(const FString& cdkey, const FString& roleid, const FString& serverid, const FSdkBoolIntStringDelegate& cdkeyCallback);
    UFUNCTION()
    void NetDiagnosis(const  FString& target, int timeout, const FSdkBoolIntStringDelegate& callback);
    UFUNCTION()
    void FetchNotice(int nScene, const FString& savePath, const FString& strLanguage, const FString& otherParams,const FSdkFetchNoticeDelegate& callback );
    //RTC
    UFUNCTION()
    FString GetRtcToken(const FString& roomId,const FString& userId,const FString& appId, const FString& appkey, int32 nExpireTime = 7200);
    UFUNCTION()
    void JoinRoom(const FString& roomId,const FString& userId, const FString& token, int roomType, int getspeakerInterval, bool enableRangeAudio, const FOnVoiceJoinRoomResult& callback);
    UFUNCTION()
    void LeaveRoom(const FOnVoiceLeaveRoom& callback );
    UFUNCTION()
    void UpdateToken(const FString& token);
    UFUNCTION()
    void SetWorldRoomReceivelRange(int disMin, int disMax);
    UFUNCTION()
    void UpdatePostion(double posX,double posY, double posZ);
    UFUNCTION()
    void EnableMicrophone(bool bEnable);
    UFUNCTION()
    void EnableAudioReceive( const FString& userId, bool bEnable);
    UFUNCTION()
    void EnableSpeakerphone(bool bEnable);
    UFUNCTION()
    void SetGetSpeakersCallback(const FOnVoiceSpeakerList& callback);
    UFUNCTION()
    void SetSpeakerVolumeChangedCallback(const FOnSpeakerVolumeChanged& callback);
    UFUNCTION()
    void SetRecordingVolume(int volume);
    UFUNCTION()
    void SetPlaybackVolume(int volume,const FString& userId);
    UFUNCTION()
    void SetUserInfoForSecureService(const FString& roleId,const FString& serverId);
    UFUNCTION()
    void AddLevelNameToGPM(const FString& strLevelName);
    UFUNCTION()
    bool IsPermissionGranted(const FString& strPermission);
    UFUNCTION()
    void OpenAppSetting();
    UFUNCTION()
    bool RequestPermission(const FString& strPermission, bool bFirstRequest);
    UFUNCTION()
    bool IsNotificationsAllowed(const FString& strChannelName);
    UFUNCTION()
    void GetCountryCode(const FSdkBoolIntThreeStringsDelegate& callback);
    UFUNCTION()
    void ChangeLanguage(const FString& langType);
    UFUNCTION()
    void ShowRatingUi();
    UFUNCTION()
    void ShowRatingCustomUi();
    UFUNCTION()
    void DismissRatingCustomUi();
    UFUNCTION()
    void OnRatingPraiseBtnClick();
    UFUNCTION()
    void OnRatingFeedbackBtnClick();
    UFUNCTION()
    bool IsBsdkPay();
    UFUNCTION()
    void Pay(const FString& productId, int goodType, int amount, const FString& currency,
        const FString& roleId,const FString&roleName, int roleLevel, int roleVipLevel,
        const FString& serverId,const FString& extInfo,const FSdkBoolIntThreeStringsDelegate& OnPay);
    UFUNCTION()
    void QueryProducts(const FString& productId,const FSdkBoolIntTwoStringsDelegate& OnQueryProductInfo); //查询商品价格信息
    UFUNCTION()
    void QueryGoods(const FString& roleId,const FString& serverId, bool bCurrency,const FSdkBoolIntStringDelegate&  OnQueryGoodsBack);//获取商品及游戏币活动信息
    UFUNCTION()
    void SetMinorLimit(const FString& country,const FString& content,const FSdkBoolIntStringDelegate&  OnSetMinorLimitBack);
    UFUNCTION()
    void GetMinorLimit(const FString& country,const FSdkBoolIntStringDelegate&  OnGetMinorLimitBack);
    UFUNCTION()
    void HasOpenFriendChainPermission(int32 nPlatformId, const FSdkBoolLongDelegate& OnCallBack);
    UFUNCTION()
    void GetFriendChainInfo(int32 nPlatformId, const FSdkBoolTwoIntStringDelegate& OnCallBack);
    UFUNCTION()
    bool UnZip(const FString& filePath, const FString& destDir, bool bSoZip, bool bDeleteFile = false);
    UFUNCTION()
    FString GetChannelOp();
    UFUNCTION()
    void ChannelGeneralAction(int32 code);
    UFUNCTION()
    void NetMpaStart(const FString& startLog);
    UFUNCTION()
    bool NetMpaAddAddress(const FString& host, int port,const FString& contextId, bool bChangeHost, FString& finalUrl, const FSdkDelegate& callback); //bChangeHost:evo-pre.btedgame.com->evo-pre-gna.btedgame.com
    UFUNCTION()
    void NetMpaStop(const FString& stopLog);
    UFUNCTION()
    void AddUploadLogfileFullPaths(const TArray<FString>& files);
    UFUNCTION()
    void AddUploadLogfileDirPath(const FString& strDir);
    UFUNCTION()
    void GetSdkFriends();
    UFUNCTION()
    void InviteFriend(const FString& friendID,const FString& InviteInfo);
    UFUNCTION()
    void SendMsgToFriend(const FString& friendID, const FString& msg);
    
    UPROPERTY()
    FSdkLoginDelegate       OnLoginCallBack;
    UPROPERTY()
    FSdkLoginDelegate       OnLogOutCallBack;
    FSdkLoginDelegate       OnSwitchAccountCallBack;
    FSdkLoginDelegate       OnAccountBindCallBack;
    FSdkLoginDelegate       OnAccountUnBindCallBack;
    FSdkAccountsDelegate    OnHistoryAccountsCallBack;
    FSdkDelegate            OnUpdateFileCallBack;
    UPROPERTY()
    FSdkBoolDelegate        OnBatteryChangeState;
    FSdkAgeDelegate         OnAgeGateCallback;
    FSdkBoolLongDelegate    OnDelateHistoryAccountCallBack;
    UPROPERTY()
    FSdkLongStringDelegate  OnAntiAddictionStateChange;
    FSdkBoolIntStringDelegate OnCdKeyExchangeCallback;
    UPROPERTY()
    FSdkBoolIntStringDelegate OnNetDiagnosisCallback;
    FSdkFetchNoticeDelegate     OnFetchNoticeCallback;
    FOnVoiceJoinRoomResult	OnJoinRomResult;
    FOnVoiceLeaveRoom		OnLeaveRoom;
    FOnVoiceSpeakerList		OnSpeakerList;
    FOnSpeakerVolumeChanged OnSpeakerVolumeCallback;
    UPROPERTY()
    FSdkStringDelegate      OnWebViewUrlClosed;
    UPROPERTY()
    FOnOpenUrlError         OnWebViewOpenUrlError;
    FSdkBoolIntThreeStringsDelegate   OnGetCountryCodeCallback;
    FSdkBoolIntStringDelegate   OnQueryGoodsCallback;
    FSdkBoolIntStringDelegate   OnSetMinorLimitCallback;
    FSdkBoolIntStringDelegate   OnGetMinorLimitCallback;
    FSdkBoolIntTwoStringsDelegate OnQueryProductInfoCallback;
    FSdkBoolIntThreeStringsDelegate OnPayCallback;
    FSdkBoolLongDelegate            OnGetFriendChainPermission;
    FSdkBoolTwoIntStringDelegate    OnGetFriendChainInfo;
    UPROPERTY()
    FSdkVoidDelegate                OnShowExitGameDialog;
    FSdkDelegate                    OnNetMpaAddAddressCallback;
    UPROPERTY()
    FSdkDelegate                    OnGetSdkFriendCallback;
    
#pragma endregion

    UFUNCTION()
    void Log(int nLevel, FString& strModuleName, FString& strLogContent)const;
    

    UFUNCTION()
    ULocalPlayer* GetFirstLocalPlayer();

    UFUNCTION()
    void ExecuteCommand(FString Command) const; 

    UFUNCTION()
    UGameInstance* GetGameInstance();
	
    UFUNCTION()
    UObject* LoadRes(const FString& ResPath);

    UFUNCTION()
    void LoadResAsync(const FString& ResPath, const FResLoadDelegate& Callback = FResLoadDelegate(), int32 Priority = 0, bool bCallbackAutoRelease=false);
        
    UFUNCTION()
    void LoadMultiResAsync(TArray<FString>& ResPathList, const FMutliResLoadDelegate& Callback = FMutliResLoadDelegate(), int32 Priority = 0, bool bCallbackAutoRelease=false);

    UFUNCTION()
    UObject* CreateObject(const UClass* Class);

    UFUNCTION()
    UUserWidget* CreateUserWidget(TSubclassOf<class UUserWidget> WidgetType);
    UFUNCTION()
    UUserWidget* CreateUserWidgetFromPath(const FString& ResPath, bool bAsync=true, const FResLoadDelegate& Callback = FResLoadDelegate(), int32 Priority = 0, bool bCallbackAutoRelease=false);

    UFUNCTION()
    FName LoadWorld(const FString& LevelPath);

    UFUNCTION()
    FName SwitchWorld(FName ToWorldContextHandle);

    UFUNCTION()
    void SetWorldTick(UWorld* world,bool bEnable);

    UFUNCTION()
    UWorld* GetWorldByWorldContextHandle(FName WorldContextHandle);
    
    UFUNCTION()
    FName GetCurrentWorldContextHandle();

    UFUNCTION()
    UObject* NewEmptyUObject(UObject * Outer);

    UFUNCTION()
    UUserWidget* GetListViewEntryOfItem(UListView* ListView, UObject* Item);
    
    UFUNCTION()
    UObject* GetListViewItemOfEntry(const UListView* ListView, const UUserWidget* entry);

    UFUNCTION()
    static void SetTexture2DPixel(UTexture2D* texture , int x,int y,uint8 value);

    UFUNCTION()
    void RefreshMeshPose(USkeletalMeshComponent* Mesh);

    UFUNCTION()
    bool IsAllLoadingDone() {return !IsLoading();}
    
    UFUNCTION()
    bool ExistPendingGame(const FName& WorldContextHandle);

    UFUNCTION()
    void DumpReplication();

    UFUNCTION()
    void AddToReplicationNormalActors(AActor* Actor);
    UFUNCTION()
    void RemoveReplicationFromNormalActors(AActor* Actor);

    UFUNCTION()
    bool ServerTravel(const FString& InURL, bool bAbsolute = false, bool bShouldSkipGameNotify = false);

private:
    FString GetIniFilePath(const FString& filePath);
    int32   _nLatentUuid;
    FTimerHandle _tHandle;
    double          m_lastTickTime;
    FLuaGcTickable* m_luaGcTick;

    void GetWidgetChildren(UWidget* pWidget,  TArray<UWidget*>& allChildren);

    TArray<FVector, TInlineAllocator<16>> _LargeWorldLocations;
    TArray<FDistanceVisibleLevel>         _LargeDistanceVisibleLevels;
    
    static  void SetActiveWorldContext(FWorldContext* InWorldContext);

    UPROPERTY()
    TMap<UClass*,FOnActorRelevantChange> Class2RelevantChangeCallBack;

    void OnPostLoadMapWithWorld(UWorld* World);

public:
    static UGameInstance* m_pGame;
    float _ticktime;
    TArray<UObject*>   m_rootObjs;    

    void OnWindowActivatedEvent();
    void OnWindowDeactivatedEvent();

    //call in UIWidgetMgr.lua:Init()
    UFUNCTION()
    void BindWindowEvent();

    FOnReplicateActorFail OnReplicateActorFail;

    UPROPERTY()
    FOnWindowActive OnWindowActiveEvent;
    
    UPROPERTY()
    FOnWindowDeActive OnWindowDeactiveEvent;
   
    UFUNCTION()
    void AddRelevantChangeCallback(UClass* Class, FOnActorRelevantChange Callback);

    UFUNCTION()
    void SearchPlayersAt(FVector Location,float Distance,TArray<AActor*>& Actors) const;

    UFUNCTION()
    void SearchActorsAt(FVector Location,float Distance,TArray<AActor*>& Actors, UClass* Class) const;

    UFUNCTION()
    void SearchTinderActorsAt(FVector Location,float Distance,TArray<AActor*>& Actors) const;

    UFUNCTION()
    TArray<int> MapSignAlphaTest(UImage* InMapImage, UTexture2D* AdvancedHitTexture, uint8 TestAlpha, TMap<int,FVector2D> InPosMap);
    
    UFUNCTION()
    static FText GetStringWithTranslation(FString keyString);
    
    UFUNCTION()
    void SetTextBlock(UObject* TextObject,FString str);

    UFUNCTION()
    void AddTinderActor(AActor* Actor);

    UFUNCTION()
    void RemoveTinderActor(AActor* Actor);
    
    UFUNCTION()
    static double GetCppTime();

    UPROPERTY()
    bool bNeedTranslation;

    UFUNCTION()
    FVector GetFlyLocation(const FVector StartPos,const FVector Speed,const float FlyTime,const float FallTime,const float Gravity);
    
    UFUNCTION()
    uint32 GetObjectUniqueID(UObject* InActor);

    bool FindChineseCharactor(const FString& InStr);

    void NotifyReplicateActorFail(APlayerController* PlayerController, AActor* Actor) const;

    UFUNCTION(BlueprintNativeEvent)
    void LuaReportLogToGameServer(const FString& V);

    UFUNCTION()
    void SetReportLogLevel(bool bReport, uint8 eLevel);

    UFUNCTION()
    void ForceStopBehaviorTree(UBehaviorTreeComponent* BehaviorTreeComponent);

    UFUNCTION()
    void OpenDisregardForGC();

    UFUNCTION()
    void CloseDisregardForGC();

    UFUNCTION(BlueprintNativeEvent)
    void OnCoverWidgetChange(UUserWidget* pTopParent, UUserWidget* coverPanel,UDragDropWidget* coverObj, UDragDropWidget* dragObj, int32 nDragType,int32 nDropType);
    
    FGameplayAbilityTargetDataHandle  MakeAbilityTargetDataFromMsg(int32 MsgId,void* Msg,USessionMgr* SesionMgr,AActor* Actor);
    Table* ExtractMsgFromAbilityTargetData(FGameplayAbilityTargetDataHandle Handle, USessionMgr* SesionMgr, int32 Index = 0);
    FHitResult ExtractMsgHitResultFromAbilityTargetData(FGameplayAbilityTargetDataHandle Handle);
    Table* EffectContextGetTargetDataToMsg(FGameplayEffectContextHandle EffectContext, USessionMgr* SesionMgr);
    UFUNCTION()
    void AddHitResultToTargetDataMsg(FGameplayAbilityTargetDataHandle Handle,FHitResult HitResult,FGameplayEffectContextHandle Context);
    UFUNCTION()
    AActor* GetCauserFromGESpec(const FGameplayEffectSpec& Spec);
    UFUNCTION()
    const FVector GetHitPosFromGESpec(const FGameplayEffectSpec& Spec);
    UFUNCTION()
    const float GetModAttributeMagnitudeFromSpec(const FGameplayEffectSpec& Spec, FGameplayAttribute Attribute);
    UFUNCTION()
    const float GetImpactForceFromASC(UAbilitySystemComponent* ASC);
    UFUNCTION()
    const bool IsAttributeChangeFromSpec(const FGameplayEffectSpec& Spec, FName AttributeName);
    UFUNCTION()
    const bool IsGESpecContainsTag(const FGameplayEffectSpec& Spec, const FName& InName);
    UFUNCTION()
    FGameplayTag GetGameplayTagByString(const FName& InName);
    UFUNCTION()
    float GetActiveGETimeRemaining(const FActiveGameplayEffect& ActiveGameplayEffect);
    UFUNCTION()
    FActiveGameplayEffectHandle GetActiveGEHandle(const FActiveGameplayEffect& ActiveGameplayEffect);
    UFUNCTION()
    bool GESpecGrantedHasTag(const FGameplayEffectSpec& GESpec, const FName& TagName);

    UFUNCTION()
    UObject* GetCDO(const UClass* InClass);

    UFUNCTION()
    float GetSetByCaller(const FGameplayEffectSpec& Spec, FName& InName, float InDefaultValue);
    UFUNCTION()
    float GetTagSetByCaller(const FGameplayEffectSpec& Spec, FGameplayTag& InTag, float InDefaultValue);

    UFUNCTION()
    void RefreshBindCommonUIKeys(TMap<FName,FCommonUIRedirectKeyData>& RedirectMap);

    UPROPERTY()
    TArray<ACPathVolume*> FlyPathVolumes;

    void AddFlyPathVolume(ACPathVolume* InVolume) { FlyPathVolumes.Add(InVolume); }

    FORCEINLINE TArray<ACPathVolume*>* GetFlyPathVolumes() { return &FlyPathVolumes; }
    
    UFUNCTION()
    void CloseNetConnection(UNetConnection* Connection);

    UFUNCTION()
    void CopyStringToClipboard(FString StringToCopy);

    UFUNCTION()
    void ShowMouseCursor(bool bShow = false);
};