#pragma once
#include "CoreMinimal.h"
#include "Sdk/VoiceInterface.h"
#if !UE_SERVER
class FYouMeTalkRtcInterface : public FVoiceInterface
{
public:
	FYouMeTalkRtcInterface();
	virtual ~FYouMeTalkRtcInterface();

	virtual void Init();
	virtual void UnInit();
	virtual bool IsInited(){return mSdkInited;}
	virtual void SetInit(bool bInit){mSdkInited = bInit;}
	virtual void JoinRoom(const FString& roomId,const FString& userId, const FString& token, int roomType, int getspeakerInterval, bool enableRangeAudio) override ;
	virtual void LeaveRoom() override ;
	virtual void UpdateToken(const FString& token) override ;
	virtual void SetWorldRoomReceivelRange(int disMin, int disMax) override ;
	virtual void UpdatePostion(int posX,int posY, int posZ) override ;
	virtual void EnableMicrophone(bool bEnable) override ;
	virtual void EnableAudioReceive(const FString& userId, bool bEnable) override ;
	virtual void EnableSpeakerphone(bool bEnable)  override ;
	virtual void SetRecordingVolume(int volume) override;
	virtual void SetPlaybackVolume(int volume,const FString& userId) override;

	virtual void OnJoinRoomResult(const FString& roomId, const FString& userId, int errorCode, bool isRejoined);
	virtual void OnLeaveRoom(const FString& roomId);
	virtual void OnSpeakerList(const TArray<FString>& speaks);
	virtual void OnSpeakerVolumeChanged(const FString& roomId, const FString& userId, int Volume);
private:
	bool mSdkInited;
};

#endif
