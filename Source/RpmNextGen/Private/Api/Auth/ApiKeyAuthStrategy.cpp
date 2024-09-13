#include "Api/Auth/ApiKeyAuthStrategy.h"
#include "Settings/RpmDeveloperSettings.h"

class URpmSettings;

FApiKeyAuthStrategy::FApiKeyAuthStrategy()
{
}

void FApiKeyAuthStrategy::AddAuthToRequest(TSharedPtr<FApiRequest> Request)
{
	const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
	if(RpmSettings->ApiKey.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("API Key is empty"));
		OnAuthComplete.ExecuteIfBound(false);
		return;
	}
	Request->Headers.Add(TEXT("X-API-KEY"), RpmSettings->ApiKey);
	OnAuthComplete.ExecuteIfBound(true);
}

void FApiKeyAuthStrategy::OnRefreshTokenResponse(const FRefreshTokenResponse& Response, bool bWasSuccessful)
{
}

void FApiKeyAuthStrategy::TryRefresh(TSharedPtr<FApiRequest> Request)
{
}



