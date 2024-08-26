// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

RPMNEXTGEN_API DECLARE_LOG_CATEGORY_EXTERN(LogReadyPlayerMe, Log, All);

class FRpmNextGenModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
