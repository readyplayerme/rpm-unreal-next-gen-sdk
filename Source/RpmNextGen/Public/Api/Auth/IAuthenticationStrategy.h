#pragma once

#include "CoreMinimal.h"
#include "Models/RefreshTokenResponse.h"

struct FApiRequest;

class RPMNEXTGEN_API IAuthenticationStrategy
{
public:
	virtual ~IAuthenticationStrategy();
	virtual void AddAuthToRequest(TSharedPtr<FApiRequest> ApiRequest,  TFunction<void(TSharedPtr<FApiRequest>, bool)> OnAuthenticationComplete) = 0;

	virtual void TryRefresh(TSharedPtr<FApiRequest> ApiRequest, TFunction<void(TSharedPtr<FApiRequest>, const FRefreshTokenResponse&, bool)> OnTokenRefreshed) = 0;
};

inline IAuthenticationStrategy::~IAuthenticationStrategy() = default;
