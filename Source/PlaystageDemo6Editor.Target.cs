// Biznet It 2024

using UnrealBuildTool;
using System.Collections.Generic;

public class PlaystageDemo6EditorTarget : TargetRules
{
	public PlaystageDemo6EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "PlaystageDemo6" } );
	}
}
