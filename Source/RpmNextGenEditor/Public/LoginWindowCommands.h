#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "LoginWindowStyle.h"

class RPMNEXTGENEDITOR_API FLoginWindowCommands: public TCommands<FLoginWindowCommands>
{
public:

	FLoginWindowCommands()
		: TCommands<FLoginWindowCommands>(TEXT("RpmNextGenEditor"), NSLOCTEXT("Contexts", "RpmNextGenEditor", "RpmNextGen Plugin"), NAME_None, FLoginWindowStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};
