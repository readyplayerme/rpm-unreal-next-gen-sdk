#include "DeveloperAccounts/DeveloperAccountApi.h"
#include "JsonObjectConverter.h"
#include "DeveloperAccounts/Models/ApplicationListRequest.h"
#include "DeveloperAccounts/Models/ApplicationListResponse.h"
#include "DeveloperAccounts/Models/OrganizationListRequest.h"
#include "DeveloperAccounts/Models/OrganizationListResponse.h"
#include "Interfaces/IHttpResponse.h"
#include "Settings/RpmDeveloperSettings.h"

FDeveloperAccountApi::FDeveloperAccountApi(const TSharedPtr<IAuthenticationStrategy>& InAuthenticationStrategy) : FWebApiWithAuth(InAuthenticationStrategy)
{
    if (URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>())
    {
        ApiBaseUrl = Settings->GetApiBaseUrl();
    }
}

void FDeveloperAccountApi::ListApplicationsAsync(const FApplicationListRequest& Request)
{
    // TODO find better way to get settings (or move to editor only code)
    const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
    ApiBaseUrl = RpmSettings->GetApiBaseUrl();
    const FString QueryString = BuildQueryString(Request.Params);
    const FString Url = FString::Printf(TEXT("%s/v1/applications%s"), *ApiBaseUrl, *QueryString);
    TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
    ApiRequest->Url = Url;
    OnRequestComplete.BindRaw(this, &FDeveloperAccountApi::HandleAppListResponse);
    DispatchRawWithAuth(ApiRequest);
}

void FDeveloperAccountApi::ListOrganizationsAsync(const FOrganizationListRequest& Request)
{
    // TODO find better way to get settings (or move to editor only code)
    const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
    ApiBaseUrl = RpmSettings->GetApiBaseUrl();
    const FString QueryString = BuildQueryString(Request.Params);
    const FString Url = FString::Printf(TEXT("%s/v1/organizations%s"), *ApiBaseUrl, *QueryString);
    TSharedPtr<FApiRequest> ApiRequest = MakeShared<FApiRequest>();
    ApiRequest->Url = Url;
    OnRequestComplete.BindRaw(this, &FDeveloperAccountApi::HandleOrgListResponse);
    DispatchRawWithAuth(ApiRequest);
}


void FDeveloperAccountApi::HandleAppListResponse(TSharedPtr<FApiRequest> ApiRequest, FHttpResponsePtr Response, bool bWasSuccessful)
{
    FApplicationListResponse ApplicationListResponse;
    FString Data = Response->GetContentAsString();
    if (bWasSuccessful && !Data.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(Data, &ApplicationListResponse, 0, 0))
    {
        OnApplicationListResponse.ExecuteIfBound(ApplicationListResponse, true);
        return;
    }
    OnApplicationListResponse.ExecuteIfBound(ApplicationListResponse, false);
}

void FDeveloperAccountApi::HandleOrgListResponse(TSharedPtr<FApiRequest> ApiRequest, FHttpResponsePtr Response, bool bWasSuccessful)
{
    FOrganizationListResponse OrganizationListResponse;
    if(Response.IsValid())
    {
        FString Data = Response->GetContentAsString();
        if (bWasSuccessful && !Data.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(Data, &OrganizationListResponse, 0, 0))
        {
            OnOrganizationResponse.ExecuteIfBound(OrganizationListResponse, true);
            return;
        }
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
