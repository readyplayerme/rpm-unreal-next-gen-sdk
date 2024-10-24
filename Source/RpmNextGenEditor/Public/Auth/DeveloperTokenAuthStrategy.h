#pragma once

#include "CoreMinimal.h"
#include "Api/Auth/AuthApi.h"
#include "Api/Auth/IAuthenticationStrategy.h"

struct FRefreshTokenResponse;

class RPMNEXTGENEDITOR_API DeveloperTokenAuthStrategy : public IAuthenticationStrategy
{
public:
	DeveloperTokenAuthStrategy();
	DeveloperTokenAuthStrategy(const FString& InApiKey);
	virtual ~DeveloperTokenAuthStrategy() override = default;
	
	virtual void AddAuthToRequest(TSharedPtr<FApiRequest> ApiRequest, TFunction<void(TSharedPtr<FApiRequest>, bool)> OnAuthComplete) override
	{
		if(ApiKey.IsEmpty())
		{
			UE_LOG(LogReadyPlayerMe, Error, TEXT("Api Key not set!") );
			OnAuthComplete(ApiRequest, false);
			return;
		}
		ApiRequest->Headers.Add(TEXT("X-API-Key"), ApiKey);

		OnAuthComplete(ApiRequest, true);
	}

	virtual void TryRefresh(TSharedPtr<FApiRequest> ApiRequest, TFunction<void(TSharedPtr<FApiRequest>, const FRefreshTokenResponse&, bool)> OnTokenRefreshed) override
	{
		// TODO add refresh request logic later
		if(ApiKey.IsEmpty())
		{
			UE_LOG(LogReadyPlayerMe, Error, TEXT("No API key provided for refresh") );
			OnTokenRefreshed(ApiRequest, FRefreshTokenResponse(), false);
			return;
		}
		OnTokenRefreshed(ApiRequest, FRefreshTokenResponse(), !ApiKey.IsEmpty());
	}
	
private:
	FString ApiKey;
	TSharedPtr<FApiRequest> ApiRequestToRetry;
};
