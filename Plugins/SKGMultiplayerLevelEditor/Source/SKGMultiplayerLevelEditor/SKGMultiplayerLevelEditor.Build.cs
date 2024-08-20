// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SKGMultiplayerLevelEditor : ModuleRules
{
	public SKGMultiplayerLevelEditor(ReadOnlyTargetRules Target) : base(Target)
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
				"PhysicsCore",
				"SKGDirectionalLight",
				"SKGGizmo"
				// ... add other public dependencies that you statically link with here ...
			}
			);

		/*if (Target.bBuildEditor)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"AnimGraph"
				}
			);
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"BlueprintGraph"
				}
			);
		}*/
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UMG",
				"Json",
				"JsonUtilities",
				"DeveloperSettings"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
