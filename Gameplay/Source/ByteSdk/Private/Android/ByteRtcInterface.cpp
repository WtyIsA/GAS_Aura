#include "ByteRtcInterface.h"
#if PLATFORM_ANDROID
#include "CallJaveObjectMethod.h"
FByteRtcInterface::FByteRtcInterface() : FVoiceInterface()
{
}

void FByteRtcInterface::JoinRoom(const FString& roomId, const FString& userId, const FString& token, int roomType,
	int getspeakerInterval, bool enableRangeAudio)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = FCallJaveObjectMethod::GetJavaMethod(Env,"VoiceJoinRoom", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IIZ)V");
		if(MethonId != 0)
		{
			auto javaroomId = FJavaHelper::ToJavaString(Env, roomId);
			auto javauserId = FJavaHelper::ToJavaString(Env, userId);
			auto javatoken = FJavaHelper::ToJavaString(Env, token);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javaroomId,*javauserId,*javatoken,roomType,getspeakerInterval,enableRangeAudio);
		}		
	}
}

void FByteRtcInterface::LeaveRoom()
{
	FCallJaveObjectMethod::CallVoidVoidMethod("VoiceLeaveRoom");
}

void FByteRtcInterface::UpdateToken(const FString& token)
{
	FCallJaveObjectMethod::CallVoidOneStringMethod("VoiceUpdateToken", token);
}

void FByteRtcInterface::SetWorldRoomReceivelRange(int disMin, int disMax)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = FCallJaveObjectMethod::GetJavaMethod(Env,"VoiceSetReceivelRange", "(II)V");
		if(MethonId != 0)
		{
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,disMin,disMax);
		}		
	}
}

void FByteRtcInterface::UpdatePostion(int posX, int posY, int posZ)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = FCallJaveObjectMethod::GetJavaMethod(Env,"VoiceUpdatePostion", "(III)V");
		if(MethonId != 0)
		{
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,posX,posY,posZ);
		}		
	}
}

void FByteRtcInterface::EnableMicrophone(bool bEnable)
{
	FCallJaveObjectMethod::CallVoidBoolMethod("VoiceEnableMicrophone", bEnable);
}

void FByteRtcInterface::EnableAudioReceive(const FString& userId, bool bEnable)
{
	if(JNIEnv* Env = FAndroidApplication::GetJavaEnv())
	{
		jmethodID MethonId = FCallJaveObjectMethod::GetJavaMethod(Env,"VoiceEnableAudioReceive", "(Ljava/lang/String;Z)V");
		if(MethonId != 0)
		{
			auto javauserId = FJavaHelper::ToJavaString(Env, userId);
			FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, MethonId,*javauserId,bEnable);
		}		
	}
}

void FByteRtcInterface::EnableSpeakerphone(bool bEnable)
{
	FCallJaveObjectMethod::CallVoidBoolMethod("VoiceEnableSpeakerphone", bEnable);
}

void FByteRtcInterface::SetRecordingVolume(int volume)
{
	FCallJaveObjectMethod::CallVoidIntMethod("VoiceSetRecordingVolume", volume );
}
void FByteRtcInterface::SetPlaybackVolume(int volume,const FString& userId)
{
	FCallJaveObjectMethod::CallVoidIntMethod("VoiceSetPlaybackVolume", volume );
}

#endif