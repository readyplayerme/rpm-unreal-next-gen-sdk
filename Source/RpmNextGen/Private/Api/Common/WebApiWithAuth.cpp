#include "Api/Common/WebApiWithAuth.h"
#include "RpmNextGen.h"
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

void FWebApiWithAuth::OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, const FApiRequest* ApiRequest)
{
    UE_LOG(LogReadyPlayerMe, Warning, TEXT("FWebApiWithAuth::OnProcessResponse"));
    if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        UE_LOG(LogReadyPlayerMe, Warning, TEXT("FWebApiWithAuth::OnProcessResponse OnRequestComplete.ExecuteIfBound"));
        OnRequestComplete.ExecuteIfBound(*ApiRequest, Response, bWasSuccessful);
    }
    else if(Response.IsValid() && Response->GetResponseCode() == EHttpResponseCodes::Denied && AuthenticationStrategy != nullptr)
    {
        UE_LOG(LogReadyPlayerMe, Warning, TEXT("TryRefresh"));
        AuthenticationStrategy->TryRefresh(ApiRequestData);
    }
    else
    {
        UE_LOG(LogReadyPlayerMe, Warning, TEXT("WebApi from URL %s request failed"), *Request->GetURL());
        OnRequestComplete.ExecuteIfBound(*ApiRequest, Response, false);
    }
}
