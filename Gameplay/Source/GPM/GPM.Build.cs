// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class GPM : ModuleRules
{
	public GPM(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);

		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Include"));
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
            string AndroidLibPath = Path.Combine(ModuleDirectory, "Lib", "Android");
            PublicSystemLibraryPaths.Add(AndroidLibPath);
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Android","libgpm.so"));
        }
	}
}
