// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

//参数 https://gdev.nvsgames.com/console/7634/account_service/open/detail/6997295787449277186?language=zh
//文档 https://gdev.nvsgames.com/docs/7641?language=zh

public class ByteSdk : ModuleRules
{
	public ByteSdk(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
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
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Main",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		if (Target.Platform == UnrealTargetPlatform.Android)
		{
			PublicDependencyModuleNames.Add("GPM");
			PrivateDependencyModuleNames.AddRange(new string[] { "Launch" });
			AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "ByteSdk_APL.XML"));
		}
		//else if (Target.Platform == UnrealTargetPlatform.IOS)
		{
		//	AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(ModuleDirectory, "AppleARKit_IOS_UPL.xml"));
		}
		PublicDefinitions.Add("USE_ANDROID_JNI=1");
		//UnrealPluginLanguage.cs
		//OnlineSubsystemGooglePlay_UPL.xml
		//$S(BuildDir) = TestSdk/Intermediate/Android/armv7
		//$S(PluginDir) = TestSdk\Plugins\Sdk\Source\Sdk 与xxx_apl.xml同级
	}
}
