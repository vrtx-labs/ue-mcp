#define UE_MCP_BLUEPRINT53_CORE 1
#define UE_MCP_BRIDGE_API UE_MCP_BRIDGE53_API

#include "UE_MCP_BridgeModule.h"
#include "Modules/ModuleManager.h"
#include "BridgeServer.h"
#include "Editor.h"
#include "Containers/Ticker.h"

DEFINE_LOG_CATEGORY(LogMCPBridge);

namespace
{
	TSharedPtr<FMCPBridgeServer> GBridgeServer;
}

class FUEMCPBridge53Module : public IModuleInterface
{
	virtual void StartupModule() override
	{
		const FMCPBridgePortChoice PortChoice = FMCPBridgeServer::ResolveConfiguredPort();
		GBridgeServer = MakeShared<FMCPBridgeServer>(PortChoice.Port, PortChoice.Source, PortChoice.bPinned);
		if (GBridgeServer->Start())
		{
			UE_LOG(LogMCPBridge, Log, TEXT("[UE-MCP] UE 5.3 Blueprint bridge starting on port %d"), PortChoice.Port);
		}
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([](float)
		{
			if (!GEditor || !GBridgeServer.IsValid()) return true;
			GBridgeServer->GetGameThreadExecutor().SetEditorReady();
			UE_LOG(LogMCPBridge, Log, TEXT("[UE-MCP] UE 5.3 Blueprint bridge ready"));
			return false;
		}));
	}

	virtual void ShutdownModule() override
	{
		if (GBridgeServer.IsValid())
		{
			GBridgeServer->Shutdown();
			GBridgeServer.Reset();
		}
	}
};

IMPLEMENT_MODULE(FUEMCPBridge53Module, UE_MCP_Bridge53)

#include "../../UE_MCP_Bridge/Private/MCPExternalRegistry.cpp"
#include "../../UE_MCP_Bridge/Private/HandlerRegistry.cpp"
#include "../../UE_MCP_Bridge/Private/GameThreadExecutor.cpp"
#include "../../UE_MCP_Bridge/Private/BridgeParamEcho.cpp"
#include "../../UE_MCP_Bridge/Private/BridgeStateFiles.cpp"
#include "../../UE_MCP_Bridge/Private/BridgeServer.cpp"
#include "../../UE_MCP_Bridge/Public/JsonSerializer.cpp"
#include "../../UE_MCP_Bridge/Private/Handlers/BlueprintHandlers.cpp"
#include "../../UE_MCP_Bridge/Private/Handlers/BlueprintHandlers_Graph.cpp"
#include "../../UE_MCP_Bridge/Private/Handlers/BlueprintHandlers_Functions.cpp"
#include "../../UE_MCP_Bridge/Private/Handlers/BlueprintHandlers_Search.cpp"
#include "../../UE_MCP_Bridge/Private/Handlers/BlueprintHandlers_Properties.cpp"
#include "../../UE_MCP_Bridge/Private/Handlers/BlueprintHandlers_Depth.cpp"
#include "../../UE_MCP_Bridge/Private/Handlers/BlueprintHandlers_Collision.cpp"
#include "../../UE_MCP_Bridge/Private/Handlers/DiffHandlers.cpp"

namespace
{
	TSharedPtr<FJsonValue> Ue53NotImplemented(const FString& Method)
	{
		TSharedPtr<FJsonObject> Body = MakeShared<FJsonObject>();
		Body->SetBoolField(TEXT("success"), false);
		Body->SetStringField(TEXT("error"), FString::Printf(
			TEXT("%s is unavailable on the UE 5.3 compatibility bridge (requires the UE 5.4 user-defined-type metadata API)"), *Method));
		return MakeShared<FJsonValueObject>(Body);
	}
}

TSharedPtr<FJsonValue> FBlueprintHandlers::ReorderEnumValues(const TSharedPtr<FJsonObject>&) { return Ue53NotImplemented(TEXT("reorder_enum_values")); }
TSharedPtr<FJsonValue> FBlueprintHandlers::SetEnumMetadata(const TSharedPtr<FJsonObject>&) { return Ue53NotImplemented(TEXT("set_enum_metadata")); }
TSharedPtr<FJsonValue> FBlueprintHandlers::SetStructFieldDefault(const TSharedPtr<FJsonObject>&) { return Ue53NotImplemented(TEXT("set_struct_field_default")); }
TSharedPtr<FJsonValue> FBlueprintHandlers::ReorderStructFields(const TSharedPtr<FJsonObject>&) { return Ue53NotImplemented(TEXT("reorder_struct_fields")); }
TSharedPtr<FJsonValue> FBlueprintHandlers::EditStructMetadata(const TSharedPtr<FJsonObject>&) { return Ue53NotImplemented(TEXT("edit_struct_metadata")); }

bool FBlueprintHandlers::ParsePinTypeSpec(const FString& TypeSpec, FEdGraphPinType& OutType, FString& OutError)
{
	OutType = MakePinType(TypeSpec);
	if (OutType.PinCategory != NAME_None) return true;
	OutError = FString::Printf(TEXT("Unrecognized type '%s'"), *TypeSpec);
	return false;
}

FString FBlueprintHandlers::PinTypeSpec(const FEdGraphPinType& PinType, bool& bOutRoundTrips)
{
	bOutRoundTrips = true;
	FString Result = PinType.PinCategory.ToString();
	if (!PinType.PinSubCategory.IsNone()) Result += TEXT(":") + PinType.PinSubCategory.ToString();
	if (PinType.ContainerType == EPinContainerType::Array) Result += TEXT("[]");
	else if (PinType.ContainerType == EPinContainerType::Set) Result = TEXT("set<") + Result + TEXT(">");
	else if (PinType.ContainerType == EPinContainerType::Map) Result = TEXT("map<") + Result + TEXT(">");
	return Result;
}
