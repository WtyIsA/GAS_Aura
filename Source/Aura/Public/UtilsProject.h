#pragma once

class FUtilsProject
{
public:
	static  FString GetFileMd5(const FString& path);
	static  FString GetContentMd5(const TArray<uint8>*);
	static  int32	    MountSavePaks();
	static  int32	    MountSavePaks(const FString& path);
	static  const FString& GetUserConfigPath();
	static  bool  PointInRect(const FMargin& rect, const FVector2D point);
	static  FString GetPakSavePath();
};
