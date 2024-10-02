﻿#include "Api/Common/WebApiWithAuth.h"
#include "Interfaces/IHttpResponse.h"

FWebApiWithAuth::FWebApiWithAuth() : ApiRequestData(nullptr), AuthenticationStrategy(nullptr)
{
    FWebApi();
    SetAuthenticationStrategy(nullptr);
}

FWebApiWithAuth::FWebApiWithAuth(IAuthenticationStrategy* InAuthenticationStrategy) : AuthenticationStrategy(nullptr)
{
    SetAuthenticationStrategy(InAuthenticationStrategy);
}

void FWebApiWithAuth::SetAuthenticationStrategy(IAuthenticationStrategy* InAuthenticationStrategy)
{
    AuthenticationStrategy = InAuthenticationStrategy;

    if (AuthenticationStrategy != nullptr)
    {
        AuthenticationStrategy->OnAuthComplete.BindRaw(this, &FWebApiWithAuth::OnAuthComplete);
        AuthenticationStrategy->OnTokenRefreshed.BindRaw(this, &FWebApiWithAuth::OnAuthTokenRefreshed);
    }
}

void FWebApiWithAuth::OnAuthComplete(bool bWasSuccessful)
{
    if(bWasSuccessful && ApiRequestData != nullptr)
    {
        DispatchRaw(*ApiRequestData);
        return;
    }
    OnRequestComplete.ExecuteIfBound(*ApiRequestData, FHttpResponsePtr() , false);
}

void FWebApiWithAuth::OnAuthTokenRefreshed(const FRefreshTokenResponseBody& Response, bool bWasSuccessful)
{
    if(bWasSuccessful)
    {
        const FString Key = TEXT("Authorization");
        if (ApiRequestData->Headers.Contains(Key))
        {
            ApiRequestData->Headers.Remove(Key);
        }
        ApiRequestData->Headers.Add(Key, FString::Printf(TEXT("Bearer %s"), *Response.Token));
        DispatchRaw(*ApiRequestData);
        return;
    }

    OnRequestComplete.ExecuteIfBound(*ApiRequestData, FHttpResponsePtr() , false);
}

void FWebApiWithAuth::DispatchRawWithAuth(FApiRequest& Data)
{
    this->ApiRequestData = MakeShared<FApiRequest>(Data);
    if (AuthenticationStrategy == nullptr)
    {
        DispatchRaw(Data);
        return;
    }

    AuthenticationStrategy->AddAuthToRequest(this->ApiRequestData);
}

void FWebApiWithAuth::OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, const FApiRequest& ApiRequest)
{
    if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        OnRequestComplete.ExecuteIfBound(ApiRequest, Response, bWasSuccessful);
        return;
    }
    if(EHttpResponseCodes::Denied == Response->GetResponseCode() && AuthenticationStrategy != nullptr)
    {
        AuthenticationStrategy->TryRefresh(ApiRequestData);
    }
}
