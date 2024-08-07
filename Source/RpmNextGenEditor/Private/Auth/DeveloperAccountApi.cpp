#include "Auth/DeveloperAccountApi.h"
#include "JsonObjectConverter.h"
#include "RpmSettings.h"

FDeveloperAccountApi::FDeveloperAccountApi(IAuthenticationStrategy* InAuthenticationStrategy) : FWebApiWithAuth(InAuthenticationStrategy)
{
    if (URpmSettings* Settings = GetMutableDefault<URpmSettings>())
    {
        ApiBaseUrl = Settings->ApiBaseUrl;
    }
}

void FDeveloperAccountApi::ListApplicationsAsync(const FApplicationListRequest& Request)
{
    const FString QueryString = BuildQueryString(Request.Params);
    const FString Url = FString::Printf(TEXT("%s/v1/applications%s"), *ApiBaseUrl, *QueryString);
    FApiRequest ApiRequest;
    ApiRequest.Url = Url;
    OnApiResponse.BindRaw(this, &FDeveloperAccountApi::HandleAppListResponse);
    DispatchRawWithAuth(ApiRequest);
}

void FDeveloperAccountApi::ListOrganizationsAsync(const FOrganizationListRequest& Request)
{
    const FString QueryString = BuildQueryString(Request.Params);
    const FString Url = FString::Printf(TEXT("%s/v1/organizations%s"), *ApiBaseUrl, *QueryString);
    FApiRequest ApiRequest;
    ApiRequest.Url = Url;
    OnApiResponse.BindRaw(this, &FDeveloperAccountApi::HandleOrgListResponse);
    DispatchRawWithAuth(ApiRequest);
}


void FDeveloperAccountApi::HandleAppListResponse(FString Data, bool bWasSuccessful)
{
    FApplicationListResponse ApplicationListResponse;
    if (bWasSuccessful && !Data.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(Data, &ApplicationListResponse, 0, 0))
    {
        OnApplicationListResponse.ExecuteIfBound(ApplicationListResponse, true);
        return;
    }
    OnApplicationListResponse.ExecuteIfBound(ApplicationListResponse, false);
}

void FDeveloperAccountApi::HandleOrgListResponse(FString Data, bool bWasSuccessful)
{
    FOrganizationListResponse OrganizationListResponse;
    if (bWasSuccessful && !Data.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(Data, &OrganizationListResponse, 0, 0))
    {
        OnOrganizationResponse.ExecuteIfBound(OrganizationListResponse, true);
        return;
    }
    OnOrganizationResponse.ExecuteIfBound(OrganizationListResponse, false);
}


FString FDeveloperAccountApi::BuildQueryString(const TMap<FString, FString>& Params)
{
    if (Params.Num() == 0) return FString();
    FString QueryString = TEXT("?");
    for (const auto& Param : Params)
    {
        QueryString += Param.Key + TEXT("=") + Param.Value + TEXT("&");
    }
    QueryString.RemoveFromEnd(TEXT("&"));
    return QueryString;
}
