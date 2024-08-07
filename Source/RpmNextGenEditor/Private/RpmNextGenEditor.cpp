// Copyright Epic Games, Inc. All Rights Reserved.

#include "RpmNextGenEditor.h"
#include "LevelEditor.h"
#include "LoginWindowCommands.h"
#include "SRpmDeveloperLoginWidget.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName TestWindowTabName("LoginWindow");

#define LOCTEXT_NAMESPACE "RpmNextGenEditorModule"

void FRpmNextGenEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FLoginWindowStyle::Initialize();
	FLoginWindowStyle::ReloadTextures();

	FLoginWindowCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FLoginWindowCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FRpmNextGenEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FRpmNextGenEditorModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TestWindowTabName, FOnSpawnTab::CreateRaw(this, &FRpmNextGenEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FRpmDevLoginWindow", "RPM Dev Login"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FRpmNextGenEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FLoginWindowStyle::Shutdown();

	FLoginWindowCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TestWindowTabName);
}

TSharedRef<SDockTab> FRpmNextGenEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FRpmNextGenEditorModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("RpmNextGenEditor.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(NomadTab)
		[
			SNew(SRpmDeveloperLoginWidget)
		];
}

void FRpmNextGenEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(TestWindowTabName);
}

void FRpmNextGenEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FLoginWindowCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FLoginWindowCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRpmNextGenEditorModule, RpmNextGenEditor)