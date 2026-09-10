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
			"EditorSubsystem", "GameplayTags", "Kismet", "KismetCompiler", "Projects", "StructUtils", "SubobjectDataInterface", "UnrealEd", "UE_MCP_BridgeStatus",
		});
	}
}
