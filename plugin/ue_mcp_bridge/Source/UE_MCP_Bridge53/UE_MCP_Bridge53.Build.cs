using UnrealBuildTool;

public class UE_MCP_Bridge53 : ModuleRules
{
	public UE_MCP_Bridge53(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateIncludePaths.AddRange(new string[]
		{
			System.IO.Path.Combine(ModuleDirectory, "..", "UE_MCP_Bridge", "Private"),
			System.IO.Path.Combine(ModuleDirectory, "..", "UE_MCP_Bridge", "Public"),
			System.IO.Path.Combine(EngineDirectory, "Plugins", "Editor", "EditorScriptingUtilities", "Source", "EditorScriptingUtilities", "Public"),
		});
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Json", "JsonUtilities" });
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"AnimGraph", "AssetRegistry", "AssetTools", "BlueprintEditorLibrary", "BlueprintGraph", "EditorScriptingUtilities",
			"DataValidation", "EditorSubsystem", "GameplayTags", "ImageWrapper", "Kismet", "KismetCompiler", "LevelEditor", "LevelSequence",
			"LevelSequenceEditor", "MainFrame", "MessageLog", "Projects", "PythonScriptPlugin", "RenderCore", "RHI", "Settings",
			"Slate", "SlateCore", "StructUtils", "SubobjectDataInterface", "UnrealEd", "UE_MCP_BridgeStatus",
		});

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PrivateDependencyModuleNames.Add("LiveCoding");
		}
	}
}
