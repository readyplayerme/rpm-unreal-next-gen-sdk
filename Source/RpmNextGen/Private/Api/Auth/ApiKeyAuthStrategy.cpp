#include "Api/Auth/ApiKeyAuthStrategy.h"

#include "RpmNextGen.h"
#include "Settings/RpmDeveloperSettings.h"

class URpmSettings;

FApiKeyAuthStrategy::FApiKeyAuthStrategy()
{
	if(const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>())
	{
		ApiKey = RpmSettings->ApiKey;
	}
}

FApiKeyAuthStrategy::FApiKeyAuthStrategy(const FString& InApiKey) : ApiKey(InApiKey)
{	
}
