// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class RPMNEXTGENEDITOR_API FRpmNextGenEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	
private:

	void RegisterMenus();
	void FillReadyPlayerMeMenu(UToolMenu* Menu);
	void OpenLoaderWindow();
	TSharedRef<SDockTab> OnSpawnLoaderWindow(const FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	TSharedPtr<class FUICommandList> PluginCommands;
	
};
