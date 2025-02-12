// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class Main : ModuleRules
{
	
	public Main(ReadOnlyTargetRules Target) : base(Target)
	{
		bEnableExceptions  = true;
		
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				Path.Combine(ModuleDirectory, "../ThirdParty/")  //rapidjson
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"NetCore",
				"Sockets",
				"CoreUObject",
				"Engine",
				"UMG",
				"Slate",
				"SlateCore",
			 	"InputCore", 
				"Lua", 
				"UnLua",
				"RenderCore",
				"Projects",
				"HTTP",
				"AIModule",
				"RHI", 
				"SignificanceManager", "GameplayAbilities", "GameplayTags", "GameplayTasks", "CommonUI", "EnhancedInput", "CommonInput", "CPathfinding"
				//"DBmanagerPlugin",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"OpenSSL",
				"ApplicationCore",
				"PakFile",
				"ReplicationGraph",
				"GameplayTasks",
				"NavigationSystem",
				"AIModule", "OnlineSubsystemUtils", "Niagara", "CPathfinding", "CPathfinding"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		if (Target.Type == TargetRules.TargetType.Editor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
			PrivateDependencyModuleNames.Add("UMGEditor");
		}
		
		PublicIncludePathModuleNames.AddRange(
			new string[] {
				//"ApplicationCore",
			}
		);
		bEnableUndefinedIdentifierWarnings = false;
		if (Target.Type == TargetType.Server ||  Target.Type == TargetType.Editor && Target.Platform != UnrealTargetPlatform.Mac )
		{
			PublicDependencyModuleNames.Add("Spdlog");
		}
		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PrivateDependencyModuleNames.AddRange(new string[] { "Launch" });
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			PublicAdditionalLibraries.Add("stdc++");
		}
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicDependencyModuleNames.Add("OnlineSubsystem");
			PublicDependencyModuleNames.Add("OnlineSubsystemUtils");
			PublicDependencyModuleNames.Add("OnlineSubsystemSteam");
		}
	}
}
