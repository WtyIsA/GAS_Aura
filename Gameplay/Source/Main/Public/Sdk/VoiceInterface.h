#pragma once

class MAIN_API FVoiceInterface
{
protected:
	FVoiceInterface(){}	
public:
	virtual ~FVoiceInterface(){}
	static FVoiceInterface* GetIns();
	static void SetSdkInterface(FVoiceInterface* pIns);

	virtual void Init(){};
	virtual void UnInit(){};
	virtual bool IsInited(){return true;}
	virtual void SetInit(bool bInit){}
	//volumeIndication 获取正在说话用户的 回调间隔，单位 ms，<=0 不回调 
	virtual void JoinRoom(const FString& roomId,const FString& userId, const FString& token, int roomType, int getspeakerInterval, bool enableRangeAudio) = 0;
	virtual void LeaveRoom() = 0;
	virtual void UpdateToken(const FString& token) = 0;
	
	/*
	 * 当收听者和声源距离小于 min 的时候，收听到的声音完全无衰减。
	 * 当收听者和声源距离介于 [min, max) 之间时，收听到的音量根据距离有衰减。
	 * 当收听者和声源距离大于 max 的时候，无法收听到声音。
	 */
	virtual void SetWorldRoomReceivelRange(int disMin, int disMax) = 0;
	virtual void UpdatePostion(int posX,int posY, int posZ) = 0;
	virtual void EnableMicrophone(bool bEnable) = 0;
	//设置是否 播放userId的声音
	virtual void EnableAudioReceive(const FString& userId, bool bEnable) = 0;
	//设置是否 播放其他人的声音
	virtual void EnableSpeakerphone(bool bEnable) = 0;
	virtual void SetRecordingVolume(int volume) = 0;
	virtual void SetPlaybackVolume(int volume,const FString& userId) = 0;
public:
	virtual void OnJoinRoomResult(const FString& roomId, const FString& userId, int errorCode, bool isRejoined);
	virtual void OnLeaveRoom(const FString& roomId);
	virtual void OnSpeakerList(const TArray<FString>& speaks);
	virtual void OnSpeakerVolumeChanged(const FString& roomId, const FString& userId, int Volume);
private:
	
	static FVoiceInterface* m_pVoiceInterface;
};
