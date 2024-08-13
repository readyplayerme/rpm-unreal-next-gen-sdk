#include "Api/Common/WebApiWithAuth.h"

#include "Interfaces/IHttpResponse.h"

FWebApiWithAuth::FWebApiWithAuth() : ApiRequestData(nullptr), AuthenticationStrategy(nullptr)
{
    FWebApi();
}

FWebApiWithAuth::FWebApiWithAuth(IAuthenticationStrategy* InAuthenticationStrategy) : AuthenticationStrategy(nullptr)
{
    SetAuthenticationStrategy(InAuthenticationStrategy);
}

void FWebApiWithAuth::SetAuthenticationStrategy(IAuthenticationStrategy* InAuthenticationStrategy)
{
    if(AuthenticationStrategy != nullptr)
    {
        AuthenticationStrategy->OnAuthComplete.Unbind();
        AuthenticationStrategy->OnTokenRefreshed.Unbind();
    }
    AuthenticationStrategy = InAuthenticationStrategy;
    if(AuthenticationStrategy == nullptr)
    {
        return;
    }
    AuthenticationStrategy->OnAuthComplete.BindRaw(this, &FWebApiWithAuth::OnAuthComplete);
    AuthenticationStrategy->OnTokenRefreshed.BindRaw(this, &FWebApiWithAuth::OnAuthTokenRefreshed);
}

void FWebApiWithAuth::OnAuthComplete(bool bWasSuccessful)
{
    if(bWasSuccessful && ApiRequestData != nullptr)
    {
        DispatchRaw(*ApiRequestData);
        return;
    }
    OnApiResponse.ExecuteIfBound(TEXT("Auth failed"), false);
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
        UE_LOG(LogTemp, Log, TEXT("Auth refreshed, running request"));
        DispatchRaw(*ApiRequestData);
        return;
    }

    OnApiResponse.ExecuteIfBound(TEXT("Auth failed"), false);
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

void FWebApiWithAuth::OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    //FWebApi::OnProcessResponse(Request, Response, bWasSuccessful);
    if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        OnApiResponse.ExecuteIfBound(Response->GetContentAsString(), true);
        return;
    }
    if(EHttpResponseCodes::Denied == Response->GetResponseCode() && AuthenticationStrategy != nullptr)
    {
        AuthenticationStrategy->TryRefresh(ApiRequestData);
    }
}
