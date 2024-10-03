#include "Api/Common/WebApi.h"
#include "HttpModule.h"
#include "RpmNextGen.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Interfaces/IHttpResponse.h"

FWebApi::FWebApi()
{
    Http = &FHttpModule::Get();
}

FWebApi::~FWebApi()
{
    
}

void FWebApi::DispatchRaw(const FApiRequest& Data)
{
    CurrentRequest = Http->CreateRequest();
    FString Url = Data.Url + BuildQueryString(Data.QueryParams);
    CurrentRequest->SetURL(Url);
    CurrentRequest->SetVerb(Data.GetVerb());
    CurrentRequest->SetTimeout(10);
    FString Headers;
    for (const auto& Header : Data.Headers)
    {
        CurrentRequest->SetHeader(Header.Key, Header.Value);
        Headers.Append(FString::Printf(TEXT("%s: %s\n"), *Header.Key, *Header.Value));
    }

    if (!Data.Payload.IsEmpty() && Data.Method != ERequestMethod::GET)
    {
        CurrentRequest->SetContentAsString(Data.Payload);
    }
    CurrentRequest->OnProcessRequestComplete().BindRaw(this, &FWebApi::OnProcessResponse, &Data);
    UE_LOG( LogTemp, Warning, TEXT("Process request to url %s with headers %s"), *CurrentRequest->GetURL(), *Headers);
    CurrentRequest->ProcessRequest();
}

void FWebApi::OnProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, const FApiRequest* ApiRequest)
{
    if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        OnRequestComplete.ExecuteIfBound(*ApiRequest, Response, true);
        return;
    }
    FString ErrorMessage = Response.IsValid() ? Response->GetContentAsString() : TEXT("Request failed");
    UE_LOG(LogReadyPlayerMe, Warning, TEXT("WebApi from URL %s request failed: %s"), *Request->GetURL(), *ErrorMessage);
    OnRequestComplete.ExecuteIfBound(*ApiRequest, Response, false);
}

FString FWebApi::BuildQueryString(const TMap<FString, FString>& QueryParams)
{
    FString QueryString;
    if (QueryParams.Num() > 0)
    {
        QueryString.Append(TEXT("?"));
        for (const auto& Param : QueryParams)
        {
            QueryString.Append(FString::Printf(TEXT("%s=%s&"), *Param.Key, *Param.Value));
        }
        QueryString.RemoveFromEnd(TEXT("&"));
    }
    QueryString = QueryString.Replace( TEXT(" "), TEXT("%20") );
    return QueryString;
}
