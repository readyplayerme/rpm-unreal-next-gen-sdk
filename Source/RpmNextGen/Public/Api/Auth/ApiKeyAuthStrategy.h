﻿#pragma once

#include "CoreMinimal.h"
#include "Api/Auth/IAuthenticationStrategy.h"
#include "Api/Common/WebApi.h"

class RPMNEXTGEN_API FApiKeyAuthStrategy : public IAuthenticationStrategy
{
public:
	FApiKeyAuthStrategy();
	virtual void AddAuthToRequest(TSharedPtr<FApiRequest> Request) override;
	virtual void OnRefreshTokenResponse(const FRefreshTokenResponse& Response, bool bWasSuccessful) override;
	virtual void TryRefresh(TSharedPtr<FApiRequest> Request) override;
};
