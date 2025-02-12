// Copyright Epic Games, Inc. All Rights Reserved.

#include "SoundDevice.h"

#if USER_AKAUDIO

#include "AkAudio/Classes/AkAudioEvent.h"
#include "AkAudio/Classes/AkGameplayStatics.h"
#include "Kismet/GameplayStatics.h"
#include "Main/Public/LuaManger.h"
#include "Sound/SlateSound.h"
#include "ActiveSound.h"
#include "AudioDevice.h"
#include "Sound/SoundBase.h"

DEFINE_LOG_CATEGORY_STATIC(LogSlateSoundDevice, Log, All);

void FSoundDevice::PlaySound(const FSlateSound& Sound, int32 UserIndex) const
{
	if (GEngine && Sound.GetResourceObject() != nullptr)
	{
		UObject* const Object = Sound.GetResourceObject();
		UAkAudioEvent* const AkAudio_Event_Const = Cast<UAkAudioEvent>(Object);
		if (AkAudio_Event_Const)
		{
			UAkAudioEvent* AkAudio_Event = const_cast<UAkAudioEvent*>(AkAudio_Event_Const);
			UAkGameplayStatics::PostEventAttached(AkAudio_Event, UGameplayStatics::GetPlayerCameraManager(GEngine->GetWorldContextFromHandle(WorldHandle)->World(), 0), NAME_None, true);
		}
		else if (Object)
		{
			FAudioDeviceHandle AudioDevice = GEngine->GetActiveAudioDevice();
			if (AudioDevice)
			{
				USoundBase* const SoundResource = Cast<USoundBase>(Object);
				if (SoundResource)
				{
					FActiveSound NewActiveSound;
					NewActiveSound.SetSound(SoundResource);
					NewActiveSound.bIsUISound = true;
					NewActiveSound.UserIndex = UserIndex;
					NewActiveSound.Priority = SoundResource->Priority;

					AudioDevice->AddNewActiveSound(NewActiveSound);
				}
				else
				{
					// An Object but no SoundResource means that the FSlateSound is holding an invalid object; report that as an error
					UE_LOG(LogSlateSoundDevice, Error, TEXT("A sound contains a non-sound resource '%s'"), *Object->GetName());
				}
			}
		}
	}
}

float FSoundDevice::GetSoundDuration(const FSlateSound& Sound) const
{
	return 0.0f;
}

#endif