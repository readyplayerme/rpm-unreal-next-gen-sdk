#pragma once


#include "CoreMinimal.h"
#include "RpmSettingsData.h" // Include your settings header

class RpmSettings
{
public:
	// Static method to get the settings instance
	static URpmSettingsData* GetRpmSettings();

private:
	// Static instance of the settings
	static URpmSettingsData* SettingsInstance;
};
