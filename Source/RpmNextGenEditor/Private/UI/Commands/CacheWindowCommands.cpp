#include "UI/Commands/CacheWindowCommands.h"

#define LOCTEXT_NAMESPACE "FRpmNextGenEditorModule"

void FCacheWindowCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Cache window", "Bring up RPM Cache window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
