#include "LoginWindowCommands.h"

#define LOCTEXT_NAMESPACE "FRpmNextGenEditorModule"

void FLoginWindowCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Rpm Developer window", "Bring up RPM Developer window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
