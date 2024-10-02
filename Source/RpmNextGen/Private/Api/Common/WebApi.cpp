#include "Api/Common/WebApi.h"
#include "HttpModule.h"
#include "RpmNextGen.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Interfaces/IHttpResponse.h"

FWebApi::FWebApi()
{
    Http = &FHttpModule::Get();
}

FWebApi::~FWebApi() {}

void FWebApi::DispatchRaw(const FApiRequest& Data)
{
    TSharedRef<IHttpRequest> Request = Http->CreateRequest();
    const FString& Url = Data.Url + BuildQueryString(Data.QueryParams);
    Request->SetURL(Url);
    Request->SetVerb(Data.GetVerb());

    for (const auto& Header : Data.Headers)
    {
        Request->SetHeader(Header.Key, Header.Value);
    }

    if (!Data.Payload.IsEmpty())
    {
        Request->SetContentAsString(Data.Payload);
    }
    TWeakPtr<FWebApi> WeakThisPtr = AsShared();
    Request->OnProcessRequestComplete().BindLambda([WeakThisPtr, Data](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        if (TSharedPtr<FWebApi> StrongThis = WeakThisPtr.Pin())
        {
            StrongThis->OnProcessResponse(Request, Response, bWasSuccessful, Data);
        }
    });
    Request->ProcessRequest();
}

void FWebApi::OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, const FApiRequest& ApiRequest)
{
    if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        OnRequestComplete.ExecuteIfBound(ApiRequest, Response, true);
        return;
    }
    FString ErrorMessage = Response.IsValid() ? Response->GetContentAsString() : TEXT("Request failed");
    UE_LOG(LogReadyPlayerMe, Warning, TEXT("WebApi from URL %s request failed: %s"), *Request->GetURL(), *ErrorMessage);
    OnRequestComplete.ExecuteIfBound(ApiRequest, Response, false);
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
