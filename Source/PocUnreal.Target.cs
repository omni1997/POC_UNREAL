using UnrealBuildTool;

public class PocUnrealTarget : TargetRules
{
	public PocUnrealTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("PocUnreal");
	}
}
