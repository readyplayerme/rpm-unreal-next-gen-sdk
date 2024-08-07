#include "Auth/DeveloperTokenAuthStrategy.h"
#include "Auth/DevAuthTokenCache.h"
#include "Api/Auth/ApiRequest.h"
#include "Api/Auth/Models/RefreshTokenResponse.h"

DeveloperTokenAuthStrategy::DeveloperTokenAuthStrategy() 
{
	AuthWebApi = FWebApiWithAuth();
	AuthWebApi.OnApiResponse.BindRaw(this, &DeveloperTokenAuthStrategy::OnRefreshTokenResponse);
	AuthWebApi.SetAuthenticationStrategy(this);
}

void DeveloperTokenAuthStrategy::AddAuthToRequest(FApiRequest& Request) 
{
	Request.Headers.Add(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *DevAuthTokenCache::GetAuthData().Token));
	OnAuthComplete.ExecuteIfBound(true, false);
}

void DeveloperTokenAuthStrategy::TryRefresh(FApiRequest& Request)
{
	if (!Request.Headers.Contains(TEXT("Authorization")))
	{
		return;
	}

	FRefreshTokenRequest RefreshRequest;
	RefreshRequest.Payload.Token = DevAuthTokenCache::GetAuthData().Token;
	RefreshRequest.Payload.RefreshToken = DevAuthTokenCache::GetAuthData().RefreshToken;

	RefreshTokenAsync(RefreshRequest);
}

void DeveloperTokenAuthStrategy::OnRefreshTokenResponse(FString Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		// Log or handle failure here if needed
		OnAuthComplete.ExecuteIfBound(false, true);
		return;
	}
    
	FRefreshTokenResponse RefreshTokenResponse;
	const bool bParsedSuccessfully = FJsonObjectConverter::JsonObjectStringToUStruct(Response, &RefreshTokenResponse, 0, 0);

	if (bParsedSuccessfully && !RefreshTokenResponse.Data.Token.IsEmpty())
	{
		DevAuthTokenCache::GetAuthData().Token = RefreshTokenResponse.Data.Token;
		UE_LOG(LogTemp, Warning, TEXT("DeveloperTokenAuthStrategy Token refreshed successfully: %s"), *RefreshTokenResponse.Data.Token);
		OnAuthComplete.ExecuteIfBound(true, true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to refresh token"));
		OnAuthComplete.ExecuteIfBound(false, true);
	}
}


void DeveloperTokenAuthStrategy::RefreshTokenAsync(const FRefreshTokenRequest& Request)
{
	FApiRequest ApiRequest = FApiRequest();
	ApiRequest.Url = TEXT("https://readyplayer.me/api/auth/refresh");
	ApiRequest.Method = POST;
	ApiRequest.Headers.Add(TEXT("Content-Type"), TEXT("application/json"));
	ApiRequest.Payload = Request.ToJsonString();
	
	AuthWebApi.DispatchRawWithAuth(ApiRequest);
}
