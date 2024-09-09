// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

RPMNEXTGEN_API DECLARE_LOG_CATEGORY_EXTERN(LogReadyPlayerMe, Log, All);

class RPMNEXTGEN_API FRpmNextGenModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// Get the global asset cache path
	static const FString& GetGlobalAssetCachePath()
	{
		return AssetCachePath;
	}

private:
	// Initialize the asset cache path
	void InitializeGlobalPaths();

	// Store the global asset cache path
	static FString AssetCachePath;
};
