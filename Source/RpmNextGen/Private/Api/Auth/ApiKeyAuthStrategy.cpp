#include "Api/Auth/ApiKeyAuthStrategy.h"
#include "Settings/RpmDeveloperSettings.h"

class URpmSettings;

FApiKeyAuthStrategy::FApiKeyAuthStrategy()
{
}

void FApiKeyAuthStrategy::AddAuthToRequest(TSharedPtr<FApiRequest> Request)
{
	URpmDeveloperSettings *Settings = GetMutableDefault<URpmDeveloperSettings>(); 
	Request->Headers.Add(TEXT("X-API-KEY"), Settings->ApiKey);
	UE_LOG(LogTemp, Warning, TEXT("Adding ApiKey to request, onauthcomplete bound: %d"), OnAuthComplete.IsBound());
	OnAuthComplete.ExecuteIfBound(true);
}

void FApiKeyAuthStrategy::OnRefreshTokenResponse(const FRefreshTokenResponse& Response, bool bWasSuccessful)
{
}

void FApiKeyAuthStrategy::TryRefresh(TSharedPtr<FApiRequest> Request)
{
	UE_LOG(LogTemp, Log, TEXT("Trying refresh"));
}



