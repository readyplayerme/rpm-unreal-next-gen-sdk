﻿#include "Api/Common/WebApiWithAuth.h"
#include "RpmNextGen.h"
#include "Interfaces/IHttpResponse.h"

FWebApiWithAuth::FWebApiWithAuth() : AuthenticationStrategy(nullptr)
{
    FWebApi();
    SetAuthenticationStrategy(nullptr);
}

FWebApiWithAuth::FWebApiWithAuth(const TSharedPtr<IAuthenticationStrategy>& InAuthenticationStrategy) : AuthenticationStrategy(nullptr)
{
    SetAuthenticationStrategy(InAuthenticationStrategy);
}

void FWebApiWithAuth::SetAuthenticationStrategy(const TSharedPtr<IAuthenticationStrategy>& InAuthenticationStrategy)
{
    AuthenticationStrategy = InAuthenticationStrategy;
    if (AuthenticationStrategy.IsValid())
    {
        AuthenticationStrategy->OnAuthComplete.Unbind();
        AuthenticationStrategy->OnTokenRefreshed.Unbind();
        
        AuthenticationStrategy->OnAuthComplete.BindRaw(this, &FWebApiWithAuth::OnAuthComplete);
        AuthenticationStrategy->OnTokenRefreshed.BindRaw(this, &FWebApiWithAuth::OnAuthTokenRefreshed);
    }
}

void FWebApiWithAuth::OnAuthComplete(TSharedPtr<FApiRequest> ApiRequest, bool bWasSuccessful)
{
    if(bWasSuccessful && ApiRequest.IsValid())
    {
        DispatchRaw(ApiRequest);
        return;
    }
    OnRequestComplete.ExecuteIfBound(ApiRequest, FHttpResponsePtr() , false);
}

void FWebApiWithAuth::OnAuthTokenRefreshed(TSharedPtr<FApiRequest> ApiRequest, const FRefreshTokenResponseBody& Response, bool bWasSuccessful)
{
    if(bWasSuccessful && ApiRequest.IsValid())
    {
        const FString Key = TEXT("Authorization");
        if (ApiRequest->Headers.Contains(Key))
        {
            ApiRequest->Headers.Remove(Key);
        }
        ApiRequest->Headers.Add(Key, FString::Printf(TEXT("Bearer %s"), *Response.Token));
        DispatchRaw(ApiRequest);
        return;
    }

    OnRequestComplete.ExecuteIfBound(ApiRequest, FHttpResponsePtr() , false);
}

void FWebApiWithAuth::DispatchRawWithAuth(TSharedPtr<FApiRequest> ApiRequest)
{
    if (AuthenticationStrategy == nullptr)
    {
        DispatchRaw(ApiRequest);
        return;
    }

    AuthenticationStrategy->AddAuthToRequest(ApiRequest);
}

void FWebApiWithAuth::OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, TSharedPtr<FApiRequest> ApiRequest)
{
    if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        OnRequestComplete.ExecuteIfBound(ApiRequest, Response, true);
    }
    else if(Response.IsValid() && Response->GetResponseCode() == EHttpResponseCodes::Denied && AuthenticationStrategy != nullptr)
    {
        AuthenticationStrategy->TryRefresh(ApiRequest);
    }
    else
    {
        UE_LOG(LogReadyPlayerMe, Warning, TEXT("WebApi from URL %s request failed"), *Request->GetURL());
        OnRequestComplete.ExecuteIfBound(ApiRequest, Response, false);
    }
}
