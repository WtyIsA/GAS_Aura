#include "YouMeTalkRtcInterface.h"

#include "YouMeTalkAPI.h"
#if !UE_SERVER

#define YOUMEAPPKEY TEXT("YOUME918279903B10A316285F5921D5B6BA98D20A889B")
#define YOUMESECRET TEXT("wAbQ8M3aiBggaGrh8aoPZHkmHhJR4aR/oaV5Ed1427/1gjnCW62kuB5Uc4Mb49eVzyK6vG69wewzKHQhHgf5RSk6O/e0KFmvMC0+ReYIRyjECBM01mFePU9FqMbFmsMM2Dh9W0BN4GATLf3yL7XvF80glaWnQ0er22m1DmIJwMEBAAE=")
FYouMeTalkRtcInterface::FYouMeTalkRtcInterface() : FVoiceInterface()
{
	mSdkInited = false;
	///TODO: YOUMESECRET 暴露有风险？
}

FYouMeTalkRtcInterface::~FYouMeTalkRtcInterface()
{
	
}

void FYouMeTalkRtcInterface::Init()
{
	if(!UYouMeTalkAPI::IsInited())
		UYouMeTalkAPI::Init(YOUMEAPPKEY,YOUMESECRET, EYouMeRTCServerRegion::RTC_DEFAULT_SERVER, TEXT(""));
}

void FYouMeTalkRtcInterface::UnInit()
{
	mSdkInited = false;
	UYouMeTalkAPI::LeaveChannelAll();
	UYouMeTalkAPI::UnInit();
}



void FYouMeTalkRtcInterface::JoinRoom(const FString& roomId, const FString& userId, const FString& token, int roomType,
	int getspeakerInterval, bool enableRangeAudio)
{
	if(!mSdkInited)
	{
		UE_LOG(LogCore,Warning,TEXT("YouMeTalk not Init"));
		Init();
		AsyncTask(ENamedThreads::GameThread, [this,roomId, userId]()
		{			
			this->OnJoinRoomResult(roomId, userId, YOUME_ERROR_NOT_INIT, false );
		});
		return;
	}
	if(UYouMeTalkAPI::IsInCertainChannel(roomId))
	{
		UE_LOG(LogCore,Verbose,TEXT("YouMeTalk IsInCertainChannel %s"), *roomId);
		return;
	}
	EYouMeErrorCode ret =  UYouMeTalkAPI::JoinChannelSingleMode(userId, roomId,EYouMeUserRole::YOUME_USER_TALKER_FREE, false );
	if( EYouMeErrorCode::YOUME_SUCCESS != ret)
	{
		UE_LOG(LogCore,Warning,TEXT("YouMeTalk JoinChannelSingleMode error=%d"),ret);
		AsyncTask(ENamedThreads::GameThread, [this,roomId, userId]()
		{			
			this->OnJoinRoomResult(roomId, userId, YOUME_ERROR_NOT_INIT, false );
		});
	}
	else
	{
		UYouMeTalkAPI::SetFarendVoiceLevelCallback(getspeakerInterval, getspeakerInterval);
		UYouMeTalkAPI::Talk_SetMicLevelCallback(getspeakerInterval);
	}
	
}

void FYouMeTalkRtcInterface::LeaveRoom()
{
	if(!mSdkInited)
	{
		UE_LOG(LogCore,Warning,TEXT("YouMeTalk not Init"));
		return;
	}
	UYouMeTalkAPI::LeaveChannelAll();
} 

void FYouMeTalkRtcInterface::UpdateToken(const FString& token)
{
	UYouMeTalkAPI::GetChannelUserList(token, -1, false);
}

void FYouMeTalkRtcInterface::SetWorldRoomReceivelRange(int disMin, int disMax)
{
	
}

void FYouMeTalkRtcInterface::UpdatePostion(int posX, int posY, int posZ)
{
	
}

void FYouMeTalkRtcInterface::EnableMicrophone(bool bEnable)
{
	if(!mSdkInited)
	{
		UE_LOG(LogCore,Warning,TEXT("YouMeTalk not Init"));
		return;
	}
	UYouMeTalkAPI::SetMicrophoneMute(!bEnable);
}

void FYouMeTalkRtcInterface::EnableAudioReceive(const FString& userId, bool bEnable)
{
	UYouMeTalkAPI::SetListenOtherVoice(userId, bEnable);
}

void FYouMeTalkRtcInterface::EnableSpeakerphone(bool bEnable)
{
	if(!mSdkInited)
	{
		UE_LOG(LogCore,Warning,TEXT("YouMeTalk not Init"));
		return;
	}
	UYouMeTalkAPI::SetSpeakerMute(!bEnable);
}

void FYouMeTalkRtcInterface::SetRecordingVolume(int volume)
{
	UYouMeTalkAPI::SetMicVolume(volume);
}
void FYouMeTalkRtcInterface::SetPlaybackVolume(int volume,const FString& userId)
{
	UYouMeTalkAPI::SetUserVolume(userId, volume);
}

void FYouMeTalkRtcInterface::OnLeaveRoom(const FString& roomId)
{
	if(!mSdkInited)
	{
		UE_LOG(LogCore,Warning,TEXT("YouMeTalk not Init"));
		return;
	}
	FVoiceInterface::OnLeaveRoom(roomId);
}

void FYouMeTalkRtcInterface::OnJoinRoomResult(const FString& roomId, const FString& userId, int errorCode, bool isRejoined)
{
	if(!mSdkInited)
	{
		UE_LOG(LogCore,Warning,TEXT("YouMeTalk not Init"));
		return;
	}
	FVoiceInterface::OnJoinRoomResult(roomId, userId, errorCode, isRejoined);
}

void FYouMeTalkRtcInterface::OnSpeakerList(const TArray<FString>& speaks)
{
	if(!mSdkInited)
	{
		UE_LOG(LogCore,Warning,TEXT("YouMeTalk not Init"));
		return;
	}
	FVoiceInterface::OnSpeakerList(speaks);
}

void FYouMeTalkRtcInterface::OnSpeakerVolumeChanged(const FString& roomId, const FString& userId, int Volume)
{
	if(!mSdkInited)
	{
		UE_LOG(LogCore,Warning,TEXT("YouMeTalk not Init"));
		return;
	}
	FVoiceInterface::OnSpeakerVolumeChanged(roomId, userId, Volume);
}



#endif