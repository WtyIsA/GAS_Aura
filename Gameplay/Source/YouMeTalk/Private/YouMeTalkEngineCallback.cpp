// Copyright © 2014-2021 YouMe. All Rights Reserved.

#include "YouMeTalkEngineCallback.h"
#include "YouMeTalkObserver.h"
#include "YouMeTalkDefine.h"
#include "Json.h"
#include "YouMeTalk.h"
#include "YouMeTalkAPI.h"
#include "YouMeTalkRtcInterface.h"


FYouMeTalkCallback* FYouMeTalkCallback::YimWork;

FYouMeTalkCallback * FYouMeTalkCallback::GetInstance()
{
	if (YimWork == nullptr)
	{
		YimWork = new FYouMeTalkCallback();
	}
	return YimWork;
}

void FYouMeTalkCallback::Destroy()
{
	if (YimWork != nullptr)
	{
		delete YimWork;
		YimWork = nullptr;
	}
}

void FYouMeTalkCallback::BindObserver(TScriptInterface<IYouMeTalkObserver> Listener)
{
	
}

void FYouMeTalkCallback::UnbindObserver()
{
	
}

void FYouMeTalkCallback::onEvent(const YouMeEvent event, const YouMeErrorCode error, const char * channel, const char * param)
{
	const EYouMeErrorCode errorCode = YouMeTalkDefineConvert::YouMeErrorCodeConvert(error);
	const EYouMeEvent youMeEvent = YouMeTalkDefineConvert::YouMeEventConvert(event);
	FString channel_fs = UTF8_TO_TCHAR(channel);
	FString param_fs = UTF8_TO_TCHAR(param);

	AsyncTask(ENamedThreads::GameThread, [ youMeEvent,event, error,errorCode, channel_fs, param_fs]
	{
		//UE_LOG(LogCore, Display, TEXT("YouMeTalkCallback::onEvent %d, %s,%s"), event, *channel_fs, *param_fs)
		switch (youMeEvent)
		{
		case EYouMeEvent::YOUME_EVENT_INIT_OK:
			FYouMeTalkModule::m_pYoumeVoiceInterface->SetInit(true);
			break;
		case EYouMeEvent::YOUME_EVENT_JOIN_OK:
			FYouMeTalkModule::m_pYoumeVoiceInterface->OnJoinRoomResult(channel_fs, param_fs, 0, false);
			UYouMeTalkAPI::GetChannelUserList(channel_fs, 10000, true);
			break;
		case EYouMeEvent::YOUME_EVENT_JOIN_FAILED:
			FYouMeTalkModule::m_pYoumeVoiceInterface->OnJoinRoomResult(channel_fs, param_fs, error, false);
			break;
		case EYouMeEvent::YOUME_EVENT_LEAVED_ALL:
			FYouMeTalkModule::m_pYoumeVoiceInterface->OnLeaveRoom(channel_fs);
			break;
		case EYouMeEvent::YOUME_EVENT_FAREND_VOICE_LEVEL:
			FYouMeTalkModule::m_pYoumeVoiceInterface->OnSpeakerVolumeChanged(channel_fs, param_fs, error);
			//UE_LOG(LogCore, Display, TEXT("YouMeTalkCallback::onEvent voice level %d, %s,%s"), error, *channel_fs, *param_fs)
			break;
		case EYouMeEvent::YOUME_EVENT_MY_MIC_LEVEL:
			FYouMeTalkModule::m_pYoumeVoiceInterface->OnSpeakerVolumeChanged(channel_fs, param_fs, error);
			//UE_LOG(LogCore, Display, TEXT("YouMeTalkCallback::onEvent my mic level %d, %s,%s"), error, *channel_fs, *param_fs)
			break;
		default:
			break;
		}		
	});
	
}

void FYouMeTalkCallback::onRequestRestAPI(int requestID, const YouMeErrorCode & iErrorCode, const char * strQuery, const char * strResult)
{
}

void FYouMeTalkCallback::onMemberChange(const char * channel, const char * listMemberChange, bool bUpdate)
{
	if(!bUpdate)
		return;	
	FString channel_fs = channel;

	FString listMemberChange_fs = UTF8_TO_TCHAR(listMemberChange);
	//UE_LOG(LogCore, Display, TEXT("YouMeTalk onMemberChange channel=%s, info=%s, bUpdate=%d"), *channel_fs, *listMemberChange_fs, bUpdate);

	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<TCHAR>::Create(listMemberChange_fs);
	TSharedPtr<FJsonObject> JsonObject;
	FJsonSerializer::Deserialize(JsonReader, JsonObject);
	TArray<TSharedPtr<FJsonValue>> data = JsonObject->GetArrayField("memchange");

	FString channelID = JsonObject->GetStringField("channelid");

	FString userID;
	for (int i = 0; i < data.Num(); i++)
	{
		FString isJoin = data[i]->AsObject()->GetStringField("isJoin");
		if (isJoin == "false")
		{
			continue; //退出房间暂时不关心
		}
		userID = data[i]->AsObject()->GetStringField("userid");
		AsyncTask(ENamedThreads::GameThread, [ channel_fs, userID]
		{
			FYouMeTalkModule::m_pYoumeVoiceInterface->OnJoinRoomResult(channel_fs, userID, 0, false);
        		//IYouMeTalkObserver::Execute_OnMemberChange(tmpObt, channel_fs, listMemberChangeMap, channelMemberStates, bUpdate);
		});
	}
	
	
	
}

void FYouMeTalkCallback::onPcmDataRemote(int channelNum, int samplingRateHz, int bytesPerSample, void * data, int dataSizeInByte)
{
}

void FYouMeTalkCallback::onPcmDataRecord(int channelNum, int samplingRateHz, int bytesPerSample, void * data, int dataSizeInByte)
{
}

void FYouMeTalkCallback::onPcmDataMix(int channelNum, int samplingRateHz, int bytesPerSample, void * data, int dataSizeInByte)
{
}
