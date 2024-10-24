#include "Api/Common/WebApi.h"

FWebApi::FWebApi()
{
    HttpModule = &FHttpModule::Get();
}

FWebApi::~FWebApi()
{
    CancelAllRequests();
}

void FWebApi::CancelAllRequests()
{
    for (const auto& Request : ActiveRequests)
    {
        Request->OnProcessRequestComplete().Unbind();
        Request->CancelRequest();
    }
    ActiveRequests.Empty();
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
