// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ARogueLikeDemo : ModuleRules
{
	public ARogueLikeDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"ARoguelikeDemo"
			}
		);
		
		//PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput","Niagara","GameplayTags","AIModule","OnlineSubsystem", "DeveloperSettings" });

		//PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput","OnlineSubsystem","DeveloperSettings","Core", "CoreUObject" });

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "EnhancedInput","SignificanceManager","OnlineSubsystem" });

		PrivateDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput","Niagara","GameplayTags","OnlineSubsystem", "DeveloperSettings","Niagara"});
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
