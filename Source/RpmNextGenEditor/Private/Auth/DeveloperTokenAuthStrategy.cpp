﻿#include "Auth/DeveloperTokenAuthStrategy.h"
#include "RpmNextGen.h"
#include "Auth/DevAuthTokenCache.h"
#include "Api/Auth/Models/RefreshTokenRequest.h"
#include "Api/Auth/Models/RefreshTokenResponse.h"
#include "Auth/Models/DeveloperAuth.h"

DeveloperTokenAuthStrategy::DeveloperTokenAuthStrategy() 
{
	AuthApi = MakeShared<FAuthApi>();
	AuthApi->OnRefreshTokenResponse.BindRaw(this, &DeveloperTokenAuthStrategy::OnRefreshTokenResponse);
}

void DeveloperTokenAuthStrategy::AddAuthToRequest(TSharedPtr<FApiRequest> ApiRequest) 
{
	const FString Key = TEXT("Authorization");
	const FString Token = FDevAuthTokenCache::GetAuthData().Token;
	if(Token.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Token is empty"));
		OnAuthComplete.ExecuteIfBound(ApiRequest, false);
		return;
	}
	if (ApiRequest->Headers.Contains(Key))
	{
		ApiRequest->Headers.Remove(Key);
	}
	ApiRequest->Headers.Add(Key, FString::Printf(TEXT("Bearer %s"), *Token));
	OnAuthComplete.ExecuteIfBound(ApiRequest, true);
}

void DeveloperTokenAuthStrategy::TryRefresh(TSharedPtr<FApiRequest> ApiRequest)
{
	ApiRequestToRetry = ApiRequest;
	FRefreshTokenRequest RefreshRequest;
	RefreshRequest.Data.Token = FDevAuthTokenCache::GetAuthData().Token;
	RefreshRequest.Data.RefreshToken = FDevAuthTokenCache::GetAuthData().RefreshToken;
	RefreshTokenAsync(RefreshRequest);
}

void DeveloperTokenAuthStrategy::OnRefreshTokenResponse(TSharedPtr<FApiRequest> Request, const FRefreshTokenResponse& Response, bool bWasSuccessful)
{
	if (bWasSuccessful && !Response.Data.Token.IsEmpty())
	{
		FDeveloperAuth DeveloperAuth = FDevAuthTokenCache::GetAuthData();
		DeveloperAuth.Token = Response.Data.Token;
		DeveloperAuth.RefreshToken = Response.Data.RefreshToken;
		FDevAuthTokenCache::SetAuthData(DeveloperAuth);
		OnTokenRefreshed.ExecuteIfBound(ApiRequestToRetry, Response.Data, true);
		return;
	}
	OnTokenRefreshed.ExecuteIfBound(ApiRequestToRetry, Response.Data, false);
}

void DeveloperTokenAuthStrategy::RefreshTokenAsync(const FRefreshTokenRequest& Request)
{	
	AuthApi->RefreshToken(Request);
}
