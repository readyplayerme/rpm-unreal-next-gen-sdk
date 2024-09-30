#pragma once

#include "CoreMinimal.h"
#include "Api/Common/WebApiWithAuth.h"

struct FOrganizationListResponse;
struct FOrganizationListRequest;
struct FApplicationListResponse;
struct FApplicationListRequest;

DECLARE_DELEGATE_TwoParams(FOnApplicationListResponse, const FApplicationListResponse&, bool);
DECLARE_DELEGATE_TwoParams(FOnOrganizationListResponse, const FOrganizationListResponse&, bool);

class RPMNEXTGENEDITOR_API FDeveloperAccountApi : public FWebApiWithAuth
{
public:
	FOnApplicationListResponse OnApplicationListResponse;
	FOnOrganizationListResponse OnOrganizationResponse;
	
	FDeveloperAccountApi(IAuthenticationStrategy* InAuthenticationStrategy);
	void ListApplicationsAsync(const FApplicationListRequest& Request);
	void ListOrganizationsAsync(const FOrganizationListRequest& Request);

private:
	FString ApiBaseUrl;
	
	void HandleOrgListResponse(FString Data, bool bWasSuccessful);
	void HandleAppListResponse(FString Data, bool bWasSuccessful);
	
	static FString BuildQueryString(const TMap<FString, FString>& Params);
};
