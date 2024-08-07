#include "Api/Common/WebApiWithAuth.h"

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
    AuthenticationStrategy = InAuthenticationStrategy;
    UE_LOG( LogTemp, Log, TEXT("About to bind =  %d"), AuthenticationStrategy->OnAuthComplete.IsBound());

    AuthenticationStrategy->OnAuthComplete.BindRaw(this, &FWebApiWithAuth::OnAuthComplete);
    UE_LOG( LogTemp, Log, TEXT("Set auth strategy. OnAuthComplete Isbound =  %d"), AuthenticationStrategy->OnAuthComplete.IsBound());
}

void FWebApiWithAuth::OnAuthComplete(bool bWasSuccessful, bool bWasRefreshed)
{
    if(bWasSuccessful)
    {
        UE_LOG(LogTemp, Log, TEXT("Dispatching raw data"));
        DispatchRaw(*ApiRequestData);
        return;
    }
    if(!bWasRefreshed)
    {
        UE_LOG(LogTemp, Log, TEXT("Auth failed, trying to refresh"));
        AuthenticationStrategy->TryRefresh(*ApiRequestData);
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("Auth failed"));
    OnApiResponse.ExecuteIfBound(TEXT("Auth failed"), false);
}

// template <typename TRequestBody>
// void FWebApiWithAuth::DispatchWithAuth(
//     const TFApiRequest<TRequestBody>& Data
// )
// {
//     TFApiRequestBody<TRequestBody> payload = TFApiRequestBody<TRequestBody>(Data.Payload);
//     FString PayloadString = ConvertToJsonString(payload.Data);
//     DispatchRawWithAuth(TFApiRequest<FString>{Data.Url, Data.Method, Data.Headers, PayloadString});
// }


void FWebApiWithAuth::DispatchRawWithAuth(
    FApiRequest& Data
)
{
    if (AuthenticationStrategy == nullptr)
    {
        UE_LOG(LogTemp, Log, TEXT("Auth strategy is null"));
        DispatchRaw(Data);
        return;
    }
    this->ApiRequestData = &Data;
    AuthenticationStrategy->AddAuthToRequest(Data);
}
