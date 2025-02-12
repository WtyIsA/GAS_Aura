#pragma once
#include "Sdk/VoiceInterface.h"

class FDefaultVoiceInterface : public  FVoiceInterface
{
public:
	FDefaultVoiceInterface():FVoiceInterface(){}
	virtual void JoinRoom(const FString& roomId,const FString& userId, const FString& token, int roomType, int getspeakerInterval, bool enableRangeAudio) override {}
	virtual void LeaveRoom() override {}
	virtual void UpdateToken(const FString& token) override {}
	virtual void SetWorldRoomReceivelRange(int disMin, int disMax) override {}
	virtual void UpdatePostion(int posX,int posY, int posZ) override {}
	virtual void EnableMicrophone(bool bEnable) override {}
	virtual void EnableAudioReceive(const FString& userId, bool bEnable) override {}
	virtual void EnableSpeakerphone(bool bEnable)  override {}
	virtual void SetRecordingVolume(int volume) override {}
	virtual void SetPlaybackVolume(int volume,const FString& userId) override {}
};
