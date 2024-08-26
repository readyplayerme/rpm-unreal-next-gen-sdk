#include "Api/Auth/ApiKeyAuthStrategy.h"
#include "Settings/RpmDeveloperSettings.h"

class URpmSettings;

FApiKeyAuthStrategy::FApiKeyAuthStrategy()
{
}

void FApiKeyAuthStrategy::AddAuthToRequest(TSharedPtr<FApiRequest> Request)
{
	URpmDeveloperSettings *Settings = GetMutableDefault<URpmDeveloperSettings>();
	if(Settings->ApiKey.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("API Key is empty"));
		OnAuthComplete.ExecuteIfBound(false);
		return;
	}
	Request->Headers.Add(TEXT("X-API-KEY"), Settings->ApiKey);
	OnAuthComplete.ExecuteIfBound(true);
}

void FApiKeyAuthStrategy::OnRefreshTokenResponse(const FRefreshTokenResponse& Response, bool bWasSuccessful)
{
}

void FApiKeyAuthStrategy::TryRefresh(TSharedPtr<FApiRequest> Request)
{
}



