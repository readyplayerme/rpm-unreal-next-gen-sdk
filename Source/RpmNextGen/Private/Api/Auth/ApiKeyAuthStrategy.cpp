#include "Api/Auth/ApiKeyAuthStrategy.h"
#include "RpmSettings.h"

class URpmSettings;

FApiKeyAuthStrategy::FApiKeyAuthStrategy()
{
}

void FApiKeyAuthStrategy::AddAuthToRequest(FApiRequest& Request)
{
	URpmSettings *Settings = GetMutableDefault<URpmSettings>(); 
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



