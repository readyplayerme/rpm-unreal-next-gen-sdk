#include "UI/Commands/CacheWindowCommands.h"

#define LOCTEXT_NAMESPACE "FRpmNextGenEditorModule"

void FCacheWindowCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Cache Generator window", "Bring up RPM Cache Generator window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
