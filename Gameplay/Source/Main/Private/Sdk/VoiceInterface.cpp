#include "Sdk/VoiceInterface.h"
#include "DefaultVoiceInterface.h"
#include "LuaManger.h"

FVoiceInterface* FVoiceInterface::m_pVoiceInterface = nullptr;

FVoiceInterface* FVoiceInterface::GetIns()
{
	if(m_pVoiceInterface == nullptr)
		m_pVoiceInterface = new FDefaultVoiceInterface();
	return m_pVoiceInterface;
}

void FVoiceInterface::SetSdkInterface(FVoiceInterface* pIns)
{
	m_pVoiceInterface = pIns;
}

void FVoiceInterface::OnJoinRoomResult(const FString& roomId, const FString& userId, int errorCode, bool isRejoined)
{
	ULuaManger::Get()->OnJoinRomResult.ExecuteIfBound(roomId, userId, errorCode, isRejoined);
}

void FVoiceInterface::OnLeaveRoom(const FString& roomId)
{
	ULuaManger::Get()->OnLeaveRoom.ExecuteIfBound();
}

void FVoiceInterface::OnSpeakerList(const TArray<FString>& speaks)
{
	ULuaManger::Get()->OnSpeakerList.ExecuteIfBound(speaks);
}

void FVoiceInterface::OnSpeakerVolumeChanged(const FString& roomId, const FString& userId, int Volume)
{
	ULuaManger::Get()->OnSpeakerVolumeCallback.ExecuteIfBound(roomId, userId, Volume);
}

