#include "UtilsProject.h"

#include "IPlatformFilePak.h"
#include "UnLuaBase.h"
#include "LuaManger.h"
#include "ShaderCodeLibrary.h"
#include "ShaderPipelineCache.h"

FString FUtilsProject::GetFileMd5(const FString& FileAbsPath)
{
	FMD5Hash FileMD5Hash = FMD5Hash::HashFile(*FileAbsPath);
	return  LexToString(FileMD5Hash);
}

FString FUtilsProject::GetContentMd5(const TArray<uint8>* pContent)
{
	FMD5Hash Hash;
	FMD5 MD5;
	if(pContent && pContent->Num() > 0)
		MD5.Update(pContent->GetData(), pContent->Num());
	Hash.Set(MD5);
	FString str =  LexToString(Hash);
	return  str;
}

int32 FUtilsProject::MountSavePaks()
{
	FString path = GetPakSavePath();// FPaths::Combine(FPaths::ProjectPersistentDownloadDir(), TEXT("Res/ExtenPaks"));
	return MountSavePaks(path);	
}

int32 FUtilsProject::MountSavePaks(const FString& path)
{
	FPakPlatformFile* PakFileMgr=(FPakPlatformFile*)FPlatformFileManager::Get().GetPlatformFile(FPakPlatformFile::GetTypeName());
	if(PakFileMgr == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("PakFileMgr is null"));
		return 0;
	}
	if(PakFileMgr->GetLowerLevel() == nullptr)
		PakFileMgr->SetLowerLevel(&FPlatformFileManager::Get().GetPlatformFile());
	TArray<FString> pakFiles;
	pakFiles.Add(path);
	//FPlatformFileManager::Get().GetPlatformFile().FindFiles(pakFiles, *path, TEXT("pak"));
	int nPakNums =	PakFileMgr->MountAllPakFiles(pakFiles);
	//if(nPakNums > 0)
	{
		UE_LOG(LogTemp, Display, TEXT("======================== OpenLibrary Begin ========================="));
		FShaderCodeLibrary::OpenLibrary("Global", FPaths::ProjectConfigDir() + TEXT("Shaderlibs/"));
		FShaderCodeLibrary::OpenLibrary(FApp::GetProjectName(), FPaths::ProjectContentDir() + TEXT("Shaderlibs/"));
		
		UE_LOG(LogTemp, Display, TEXT("======================== OpenLibrary End ========================="));

	}
	UE_LOG(LogTemp,Display,TEXT("TextLocalization ---------------"));
	InitEngineTextLocalization();
	InitGameTextLocalization();

	if(auto Var = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ShaderPipelineCache.ForceEnable")))
	    Var->Set(1);
	bool bRe = FShaderPipelineCache::OpenPipelineFileCache(GMaxRHIShaderPlatform);

	UE_LOG(LogTemp, Log, TEXT("======================== OpenPipelineFileCache End OpenPipelineFileCache=%d, ========================="), bRe);

	return nPakNums;
}


const FString& FUtilsProject::GetUserConfigPath()
{
	static FString iniPath;
	if(iniPath.Len() == 0)
	{
		FString ProjectPersistentDownloadDir = UnLua::GetDonwloadResPath();
		iniPath = FPaths::Combine(ProjectPersistentDownloadDir, TEXT("Config/user.ini"));
		FString tempPath =FPaths::GetPath(iniPath);
		if(!IFileManager::Get().DirectoryExists(*tempPath))
		{
			IFileManager::Get().MakeDirectory(*tempPath, true);
		}			
	}
	return  iniPath;
}
bool  FUtilsProject::PointInRect(const FMargin& rect, const FVector2D point)
{
	if(point.X < rect.Left || point.X > rect.Right)
		return  false;
	if(point.Y < rect.Top || point.Y > rect.Bottom)
		return  false;
	return  true;
}

FString FUtilsProject::GetPakSavePath()
{
	return  FPaths::Combine(*FPaths::ProjectPersistentDownloadDir(), TEXT("Res/Paks/"));
}
