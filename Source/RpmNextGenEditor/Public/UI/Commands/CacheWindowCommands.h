#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class RPMNEXTGENEDITOR_API FCacheWindowCommands : public TCommands<FCacheWindowCommands>
{
public:
	FCacheWindowCommands()
		: TCommands<FCacheWindowCommands>(TEXT("CacheWindow"), NSLOCTEXT("Contexts", "CacheWindow", "Cache Window Plugin"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};