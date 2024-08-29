#include "UI/Commands/LoaderWindowCommands.h"

#define LOCTEXT_NAMESPACE "FRpmNextGenEditorModule"

void FLoaderWindowCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Open Loader Window", "Open the custom loader window", EUserInterfaceActionType::Button, FInputGesture());
}
#undef LOCTEXT_NAMESPACE