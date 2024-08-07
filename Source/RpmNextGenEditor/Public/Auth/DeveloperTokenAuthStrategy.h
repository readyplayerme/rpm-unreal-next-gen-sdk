#pragma once

#include "CoreMinimal.h"
#include "Api/Auth/ApiRequest.h"
#include "Api/Auth/IAuthenticationStrategy.h"
#include "Api/Common/WebApiWithAuth.h"
#include "Api/Auth/Models/RefreshTokenRequest.h"

struct FRefreshTokenResponse;


class RPMNEXTGENEDITOR_API DeveloperTokenAuthStrategy : public IAuthenticationStrategy
{
public:
	DeveloperTokenAuthStrategy();
	virtual void AddAuthToRequest(FApiRequest& Request) override;
	virtual void OnRefreshTokenResponse(FString Response, bool bWasSuccessful) override;
	virtual void TryRefresh(FApiRequest& Request) override;
private:
	void RefreshTokenAsync(const FRefreshTokenRequest& Request);
	FOnWebApiResponse OnWebApiResponse;
	FWebApiWithAuth AuthWebApi;
};
