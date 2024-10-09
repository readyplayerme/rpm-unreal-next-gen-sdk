// Copyright Epic Games, Inc. All Rights Reserved.

#include "RpmNextGen.h"

DEFINE_LOG_CATEGORY(LogReadyPlayerMe);

#define LOCTEXT_NAMESPACE "FRpmNextGenModule"

void FRpmNextGenModule::StartupModule()
{
}

void FRpmNextGenModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRpmNextGenModule, RpmNextGen)