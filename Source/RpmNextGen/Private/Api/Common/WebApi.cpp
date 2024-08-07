﻿#include "Api/Common/WebApi.h"
#include "HttpModule.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Interfaces/IHttpResponse.h"

FWebApi::FWebApi()
{
    Http = &FHttpModule::Get();
}

FWebApi::~FWebApi() {}

// template <typename TRequestBody>
// void FWebApi::Dispatch(const FApiRequest<TRequestBody>& Data)
// {
//     TFApiRequestBody<TRequestBody> payload = TFApiRequestBody<TRequestBody>(Data.Payload);
//     FString PayloadString = ConvertToJsonString(payload.Data);
//     DispatchRaw(FApiRequest{Data.Url, Data.Method, Data.Headers, PayloadString});
// }

void FWebApi::DispatchRaw(const FApiRequest& Data)
{
    TSharedRef<IHttpRequest> Request = Http->CreateRequest();
    Request->SetURL(Data.Url);
    Request->SetVerb(Data.GetVerb());

    for (const auto& Header : Data.Headers)
    {
        Request->SetHeader(Header.Key, Header.Value);
        //UE_LOG(LogTemp, Log, TEXT("Header Key: %s | Value: %s"), *Header.Key, *Header.Value);
    }

    if (!Data.Payload.IsEmpty())
    {
        Request->SetContentAsString(Data.Payload);
    }

    Request->OnProcessRequestComplete().BindRaw(this, &FWebApi::OnProcessResponse);
    Request->ProcessRequest();
}

void FWebApi::OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    
    if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        UE_LOG(LogTemp, Warning, TEXT("WebApi from URL %s Response Success: %s."), *Request->GetURL(), *Response->GetContentAsString());
        OnApiResponse.ExecuteIfBound(Response->GetContentAsString(), true);
        return;
    }
    FString ErrorMessage = Response.IsValid() ? Response->GetContentAsString() : TEXT("Request failed");
    UE_LOG(LogTemp, Warning, TEXT("WebApi from URL %s request failed: %s"), *Request->GetURL(), *ErrorMessage);
    OnApiResponse.ExecuteIfBound(Response->GetContentAsString(), false);
}

FString FWebApi::BuildQueryString(const TMap<FString, FString>& QueryParams)
{
    FString QueryString;
    if (QueryParams.Num() > 0)
    {
        QueryString.Append(TEXT("?"));
        for (const auto& Param : QueryParams)
        {
            QueryString.Append(FString::Printf(TEXT("%s=%s&"), *FGenericPlatformHttp::UrlEncode(Param.Key), *FGenericPlatformHttp::UrlEncode(Param.Value)));
        }
        QueryString.RemoveFromEnd(TEXT("&"));
    }
    return QueryString;
}