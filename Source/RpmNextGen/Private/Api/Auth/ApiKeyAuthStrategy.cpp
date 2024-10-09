#include "Api/Auth/ApiKeyAuthStrategy.h"

#include "RpmNextGen.h"
#include "Settings/RpmDeveloperSettings.h"

class URpmSettings;

FApiKeyAuthStrategy::FApiKeyAuthStrategy()
{
}

void FApiKeyAuthStrategy::AddAuthToRequest(TSharedPtr<FApiRequest> ApiRequest)
{
	const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
	if(RpmSettings->ApiKey.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("API Key is empty"));
		OnAuthComplete.ExecuteIfBound(ApiRequest, false);
		return;
	}
	ApiRequest->Headers.Add(TEXT("X-API-KEY"), RpmSettings->ApiKey);
	OnAuthComplete.ExecuteIfBound(ApiRequest, true);
}

void FApiKeyAuthStrategy::OnRefreshTokenResponse(TSharedPtr<FApiRequest> ApiRequest, const FRefreshTokenResponse& Response, bool bWasSuccessful)
{
	OnTokenRefreshed.ExecuteIfBound(ApiRequest, Response.Data, bWasSuccessful);
}

void FApiKeyAuthStrategy::TryRefresh(TSharedPtr<FApiRequest> ApiRequest)
{
	OnAuthComplete.ExecuteIfBound(ApiRequest, false);
}
