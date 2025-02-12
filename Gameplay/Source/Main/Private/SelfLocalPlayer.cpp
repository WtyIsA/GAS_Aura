#include "SelfLocalPlayer.h"
#include "LuaManger.h"

USelfLocalPlayer::USelfLocalPlayer()
{
    InitializeDefaults();
}

void USelfLocalPlayer::AddOption(FString Key, FString Value)
{
    OptionalData.Add(Key,Value);
    UE_LOG(GAME, Display,TEXT("UYLLocalPlayer::UYLLocalPlayer() AddOption %s,%s"),*Key,*Value);
}

void USelfLocalPlayer::RemoveOption(FString Key)
{
    OptionalData.Remove(Key);
    UE_LOG(GAME, Display,TEXT("UYLLocalPlayer::UYLLocalPlayer() RemoveOption %s"),*Key);
}

void USelfLocalPlayer::ClearAllOptions()
{
    OptionalData.Reset();
    UE_LOG(GAME, Display,TEXT("UYLLocalPlayer::UYLLocalPlayer() ClearAllOptions"));
}

FString USelfLocalPlayer::GetGameLoginOptions() const
{
    FString Options;
    bool IsFirst = true;
    for (TPair<FString, FString> Element: OptionalData)
    {
        if(IsFirst)
        {
            IsFirst = false;
        }else
        {
            Options.Append("?");
        }
        Options.Append(TCHAR_TO_UTF8(*Element.Key));
        Options.Append("=");
        Options.Append(TCHAR_TO_UTF8(*Element.Value));
    }
    UE_LOG(GAME, Display,TEXT("UYLLocalPlayer::GetGameLoginOptions() %s"),*Options);
    return Options;
}

void USelfLocalPlayer::InitializeDefaults()
{
}
