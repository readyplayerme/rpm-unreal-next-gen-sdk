﻿#pragma once

#include "CoreMinimal.h"
#include "Api/Auth/ApiRequest.h"
#include "Models/RefreshTokenResponse.h"

DECLARE_DELEGATE_TwoParams(FOnAuthComplete, bool, bool);

class RPMNEXTGEN_API IAuthenticationStrategy
{
public:
	virtual ~IAuthenticationStrategy() = default;
	virtual void AddAuthToRequest(FApiRequest& Request) = 0;
	virtual void TryRefresh(FApiRequest& Request) = 0;
	virtual void OnRefreshTokenResponse(const FRefreshTokenResponse& Response, bool bWasSuccessful) = 0;
	FOnAuthComplete OnAuthComplete;
};