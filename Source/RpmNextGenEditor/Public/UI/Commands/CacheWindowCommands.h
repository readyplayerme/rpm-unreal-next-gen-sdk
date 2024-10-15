#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

class RPMNEXTGENEDITOR_API FCacheWindowCommands : public TCommands<FCacheWindowCommands>
{
public:
	FCacheWindowCommands()
		: TCommands<FCacheWindowCommands>(TEXT("CacheGeneratorWindow"), NSLOCTEXT("Contexts", "CacheGeneratorWindow", "Cache Generator Window"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};