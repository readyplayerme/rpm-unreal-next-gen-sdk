#pragma once

#include "CoreMinimal.h"
#include "Api/Auth/IAuthenticationStrategy.h"
#include "Api/Common/WebApi.h"

class RPMNEXTGEN_API FApiKeyAuthStrategy : public IAuthenticationStrategy
{
public:
	FApiKeyAuthStrategy();
	virtual void AddAuthToRequest(TSharedPtr<FApiRequest> ApiRequest) override;
	virtual void TryRefresh(TSharedPtr<FApiRequest> ApiRequest) override;
	virtual void OnRefreshTokenResponse(TSharedPtr<FApiRequest> ApiRequest, const FRefreshTokenResponse& Response, bool bWasSuccessful) override;
};
