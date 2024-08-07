#include "Api/Auth/ApiKeyAuthStrategy.h"
#include "Settings/RpmDeveloperSettings.h"

class URpmSettings;

FApiKeyAuthStrategy::FApiKeyAuthStrategy()
{
}

void FApiKeyAuthStrategy::AddAuthToRequest(FApiRequest& Request)
{
	URpmDeveloperSettings *Settings = GetMutableDefault<URpmDeveloperSettings>(); 
	Request.Headers.Add(TEXT("X-API-KEY"), Settings->ApiKey);
	UE_LOG(LogTemp, Log, TEXT("Added API key to request %s"), *Settings->ApiKey);
	UE_LOG(LogTemp, Log, TEXT("Is OnAuthComplete bound = %d"), OnAuthComplete.IsBound());
	
	OnAuthComplete.ExecuteIfBound(true, false);
}

void FApiKeyAuthStrategy::OnRefreshTokenResponse(FString Response, bool bWasSuccessful)
{
}

void FApiKeyAuthStrategy::TryRefresh(FApiRequest& Request)
{
	UE_LOG(LogTemp, Log, TEXT("Trying refresh"));
}



