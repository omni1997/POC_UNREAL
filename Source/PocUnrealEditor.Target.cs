using UnrealBuildTool;

public class PocUnrealEditorTarget : TargetRules
{
	public PocUnrealEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("PocUnreal");
	}
}
