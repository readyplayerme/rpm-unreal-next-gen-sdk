#pragma once

#include "CoreMinimal.h"
#include "Api/Auth/ApiRequest.h"
#include "Api/Auth/AuthApi.h"
#include "Api/Auth/IAuthenticationStrategy.h"

struct FRefreshTokenResponse;


class RPMNEXTGENEDITOR_API DeveloperTokenAuthStrategy : public IAuthenticationStrategy
{
public:
	DeveloperTokenAuthStrategy();
	virtual void AddAuthToRequest(TSharedPtr<FApiRequest> Request) override;
	virtual void OnRefreshTokenResponse(const FRefreshTokenResponse& Response, bool bWasSuccessful) override;
	virtual void TryRefresh(TSharedPtr<FApiRequest> Request) override;
	
private:
	FOnWebApiResponse OnWebApiResponse;
	FAuthApi AuthApi;
	
	void RefreshTokenAsync(const FRefreshTokenRequest& Request);
};
