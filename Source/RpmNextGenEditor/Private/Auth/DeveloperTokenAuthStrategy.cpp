﻿#include "Auth/DeveloperTokenAuthStrategy.h"

#include "RpmNextGen.h"
#include "Auth/DevAuthTokenCache.h"
#include "Api/Auth/Models/RefreshTokenRequest.h"
#include "Api/Auth/Models/RefreshTokenResponse.h"
#include "Auth/Models/DeveloperAuth.h"

DeveloperTokenAuthStrategy::DeveloperTokenAuthStrategy() 
{
	AuthApi = FAuthApi();
	AuthApi.OnRefreshTokenResponse.BindRaw(this, &DeveloperTokenAuthStrategy::OnRefreshTokenResponse);
}

void DeveloperTokenAuthStrategy::AddAuthToRequest(TSharedPtr<FApiRequest> Request) 
{
	const FString Key = TEXT("Authorization");
	const FString Token = FDevAuthTokenCache::GetAuthData().Token;
	if(Token.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Token is empty"));
		OnAuthComplete.ExecuteIfBound(false);
		return;
	}
	if (Request->Headers.Contains(Key))
	{
		Request->Headers.Remove(Key);
	}
	Request->Headers.Add(Key, FString::Printf(TEXT("Bearer %s"), *Token));
	
	OnAuthComplete.ExecuteIfBound(true);
}

void DeveloperTokenAuthStrategy::TryRefresh(TSharedPtr<FApiRequest> Request)
{
	FRefreshTokenRequest RefreshRequest;
	RefreshRequest.Data.Token = FDevAuthTokenCache::GetAuthData().Token;
	RefreshRequest.Data.RefreshToken = FDevAuthTokenCache::GetAuthData().RefreshToken;
	UE_LOG(LogReadyPlayerMe, Warning, TEXT("TryRefresh"));

	RefreshTokenAsync(RefreshRequest);
}

void DeveloperTokenAuthStrategy::OnRefreshTokenResponse(const FRefreshTokenResponse& Response, bool bWasSuccessful)
{
	UE_LOG(LogReadyPlayerMe, Error, TEXT("refresh token response"));
	if (bWasSuccessful && !Response.Data.Token.IsEmpty())
	{
		FDeveloperAuth DeveloperAuth = FDevAuthTokenCache::GetAuthData();
		DeveloperAuth.Token = Response.Data.Token;
		DeveloperAuth.RefreshToken = Response.Data.RefreshToken;
		FDevAuthTokenCache::SetAuthData(DeveloperAuth);
		UE_LOG(LogReadyPlayerMe, Warning, TEXT("Refresh token success"));
		OnTokenRefreshed.ExecuteIfBound(Response.Data, true);
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to refresh token"));
	OnTokenRefreshed.ExecuteIfBound(Response.Data, false);
}


void DeveloperTokenAuthStrategy::RefreshTokenAsync(const FRefreshTokenRequest& Request)
{	
	AuthApi.RefreshToken(Request);
}
