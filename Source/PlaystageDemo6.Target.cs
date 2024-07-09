// Biznet It 2024

using UnrealBuildTool;
using System.Collections.Generic;

public class PlaystageDemo6Target : TargetRules
{
	public PlaystageDemo6Target(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "PlaystageDemo6" } );
	}
}
