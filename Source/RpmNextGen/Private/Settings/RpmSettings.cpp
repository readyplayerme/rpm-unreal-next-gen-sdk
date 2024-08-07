#include "RpmSettings.h"

URpmSettingsData* RpmSettings::SettingsInstance = nullptr;

URpmSettingsData* RpmSettings::GetRpmSettings()
{
	// Check if the settings instance is already initialized
	if (!SettingsInstance)
	{
		// Initialize the settings instance if it is not already
		SettingsInstance = GetMutableDefault<URpmSettingsData>();
	}

	// Return the settings instance
	return SettingsInstance;
}