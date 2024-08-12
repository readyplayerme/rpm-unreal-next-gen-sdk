// Copyright Epic Games, Inc. All Rights Reserved.

#include "RpmNextGenEditor.h"

#include "CharacterLoaderWidget.h"
#include "LevelEditor.h"
#include "LoaderWindowCommands.h"
#include "LoginWindowCommands.h"
#include "SRpmDeveloperLoginWidget.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName TestWindowTabName("LoginWindow");

#define LOCTEXT_NAMESPACE "RpmNextGenEditorModule"
static const FName NewWindowTabName("CustomEditorWindow");

void FRpmNextGenEditorModule::StartupModule()
{
	FLoginWindowStyle::Initialize();
	FLoginWindowStyle::ReloadTextures();

	FLoginWindowCommands::Register();
	FLoaderWindowCommands::Register();  // Don't forget to register the other command set

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FLoginWindowCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FRpmNextGenEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FLoaderWindowCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FRpmNextGenEditorModule::OpenLoaderWindow),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FRpmNextGenEditorModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TestWindowTabName, FOnSpawnTab::CreateRaw(this, &FRpmNextGenEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FRpmDevLoginWindow", "RPM Dev Login"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(NewWindowTabName, FOnSpawnTab::CreateRaw(this, &FRpmNextGenEditorModule::OnSpawnLoaderWindow))
		.SetDisplayName(LOCTEXT("CustomEditorWindow", "Custom Editor Window"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FRpmNextGenEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	// Create a new main menu entry called "ReadyPlayerMe"
	UToolMenu* MainMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu");

	// Add a new top-level menu "Ready Player Me"
	FToolMenuSection& Section = MainMenu->AddSection("ReadyPlayerMeTopMenu", LOCTEXT("ReadyPlayerMeMenuSection", "Ready Player Me"));

	// Add a sub-menu for "Ready Player Me"
	FToolMenuEntry& SubMenuEntry = Section.AddSubMenu(
		"ReadyPlayerMe",
		LOCTEXT("ReadyPlayerMeMenu", "Ready Player Me"),
		LOCTEXT("ReadyPlayerMeMenu_ToolTip", "Open Ready Player Me tools"),
		FNewToolMenuDelegate::CreateRaw(this, &FRpmNextGenEditorModule::FillReadyPlayerMeMenu),
		false, // Don't open on hover
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.User") // Optional icon for the top-level menu
	);
}

void FRpmNextGenEditorModule::FillReadyPlayerMeMenu(UToolMenu* Menu)
{
	FToolMenuSection& Section = Menu->AddSection("ReadyPlayerMeSubSection");

	Section.AddMenuEntry(
		"OpenLoginWindow",
		LOCTEXT("OpenLoginWindow", "Developer Window"),
		LOCTEXT("OpenLoginWindowToolTip", "Open the RPM Developer Window."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FRpmNextGenEditorModule::PluginButtonClicked))
	);

	Section.AddMenuEntry(
		"OpenLoaderWindow",
		LOCTEXT("OpenLoaderWindow", "Glb Loader"),
		LOCTEXT("OpenLoaderWindowToolTip", "Glb Loader Window."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FRpmNextGenEditorModule::OpenLoaderWindow))
	);
}

void FRpmNextGenEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	FLoginWindowStyle::Shutdown();

	FLoginWindowCommands::Unregister();
	FLoaderWindowCommands::Unregister(); // Unregister custom commands

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TestWindowTabName);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(NewWindowTabName);
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

TSharedRef<SDockTab> FRpmNextGenEditorModule::OnSpawnLoaderWindow(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(NomadTab)
		[
			SNew(SCharacterLoaderWidget)
		];
}

void FRpmNextGenEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(TestWindowTabName);
}

// void FRpmNextGenEditorModule::RegisterMenus()
// {
// 	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
// 	FToolMenuOwnerScoped OwnerScoped(this);
//
// 	// Create a new main menu entry
// 	UToolMenu* MainMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu");
//
// 	// Add a new section to the main menu for ReadyPlayerMe
// 	FToolMenuSection& Section = MainMenu->AddSection("ReadyPlayerMe", LOCTEXT("ReadyPlayerMeMenuSection", "Ready Player Me"));
//
// 	// Add menu entries to your custom section
// 	Section.AddMenuEntryWithCommandList(FLoginWindowCommands::Get().OpenPluginWindow, PluginCommands);
// 	Section.AddMenuEntryWithCommandList(FLoaderWindowCommands::Get().OpenPluginWindow, PluginCommands);
//
// 	// Add the section to the toolbar as well, if you want it to appear in the toolbar
// 	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
// 	{
// 		FToolMenuSection& ToolbarSection = ToolbarMenu->FindOrAddSection("ReadyPlayerMe");
// 		{
// 			FToolMenuEntry& Entry = ToolbarSection.AddEntry(FToolMenuEntry::InitToolBarButton(FLoginWindowCommands::Get().OpenPluginWindow));
// 			Entry.SetCommandList(PluginCommands);
// 		}
// 	}
// }


void FRpmNextGenEditorModule::OpenLoaderWindow()
{
	FGlobalTabmanager::Get()->TryInvokeTab(NewWindowTabName);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRpmNextGenEditorModule, RpmNextGenEditor)