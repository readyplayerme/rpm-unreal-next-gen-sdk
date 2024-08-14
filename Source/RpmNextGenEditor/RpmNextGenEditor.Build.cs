// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RpmNextGenEditor : ModuleRules
{
	public RpmNextGenEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"HTTP",
				"JsonUtilities",
				"RpmNextGen",
				"EditorStyle",
				"glTFRuntime",
				"TransientObjectSaver",
				"UnrealEd",
				"PropertyEditor",
				"Slate",
				"SlateCore",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"EditorFramework",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Json", 
				"UMG",
				"ImageWrapper",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
