#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

class FLoaderWindowCommands : public TCommands<FLoaderWindowCommands>
{
public:
	FLoaderWindowCommands()
		: TCommands<FLoaderWindowCommands>(TEXT("LoaderWindow"), NSLOCTEXT("Contexts", "LoaderWindow", "Loader Window Plugin"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};
