#pragma once

#include "CoreMinimal.h"
#include "DevAuthTokenCache.h"
#include "Api/Auth/AuthApi.h"
#include "Api/Auth/IAuthenticationStrategy.h"
#include "Api/Auth/Models/RefreshTokenRequest.h"
#include "Models/DeveloperAuth.h"

struct FRefreshTokenResponse;

class RPMNEXTGENEDITOR_API DeveloperTokenAuthStrategy : public IAuthenticationStrategy, public FAuthApi
{
public:
	DeveloperTokenAuthStrategy();
	DeveloperTokenAuthStrategy(const FString& InApiKey);
	~DeveloperTokenAuthStrategy();
	
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

	virtual void TryRefresh(TSharedPtr<FApiRequest> ApiRequest, TFunction<void(TSharedPtr<FApiRequest>, const FRefreshTokenResponse&, bool)> OnTokenRefreshed) override;
	
private:
	FString ApiKey;
	TSharedPtr<FApiRequest> ApiRequestToRetry;
};
