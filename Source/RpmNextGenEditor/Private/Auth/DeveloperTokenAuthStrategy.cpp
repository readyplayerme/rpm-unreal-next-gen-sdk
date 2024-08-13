#include "Auth/DeveloperTokenAuthStrategy.h"
#include "Auth/DevAuthTokenCache.h"
#include "Api/Auth/ApiRequest.h"
#include "Api/Auth/Models/RefreshTokenRequest.h"
#include "Api/Auth/Models/RefreshTokenResponse.h"

DeveloperTokenAuthStrategy::DeveloperTokenAuthStrategy() 
{
	AuthApi = FAuthApi();
	AuthApi.OnRefreshTokenResponse.BindRaw(this, &DeveloperTokenAuthStrategy::OnRefreshTokenResponse);
	
}

void DeveloperTokenAuthStrategy::AddAuthToRequest(FApiRequest& Request) 
{
	const FString Key = TEXT("Authorization");
	if (Request.Headers.Contains(Key))
	{
		Request.Headers.Remove(Key);
	}
	Request.Headers.Add(Key, FString::Printf(TEXT("Bearer %s"), *DevAuthTokenCache::GetAuthData().Token));
	
	OnAuthComplete.ExecuteIfBound(true);
}

void DeveloperTokenAuthStrategy::TryRefresh(FApiRequest& Request)
{
	FRefreshTokenRequest RefreshRequest;
	RefreshRequest.Data.Token = DevAuthTokenCache::GetAuthData().Token;
	RefreshRequest.Data.RefreshToken = DevAuthTokenCache::GetAuthData().RefreshToken;

	RefreshTokenAsync(RefreshRequest);
}

void DeveloperTokenAuthStrategy::OnRefreshTokenResponse(const FRefreshTokenResponse& Response, bool bWasSuccessful)
{
	if (bWasSuccessful && !Response.Data.Token.IsEmpty())
	{
		FDeveloperAuth DeveloperAuth = DevAuthTokenCache::GetAuthData();
		DeveloperAuth.Token = Response.Data.Token;
		DeveloperAuth.RefreshToken = Response.Data.RefreshToken;
		DevAuthTokenCache::SetAuthData(DeveloperAuth);
		OnTokenRefreshed.ExecuteIfBound(Response.Data, true);
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("Failed to refresh token"));
	OnTokenRefreshed.ExecuteIfBound(Response.Data, false);
}


void DeveloperTokenAuthStrategy::RefreshTokenAsync(const FRefreshTokenRequest& Request)
{	
	AuthApi.RefreshToken(Request);
}
