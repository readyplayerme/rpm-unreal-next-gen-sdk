#pragma once

#include "CoreMinimal.h"
#include "Api/Common/WebApiWithAuth.h"

struct FOrganizationListResponse;
struct FOrganizationListRequest;
struct FApplicationListResponse;
struct FApplicationListRequest;

DECLARE_DELEGATE_TwoParams(FOnApplicationListResponse, TSharedPtr<FApplicationListResponse>, bool);
DECLARE_DELEGATE_TwoParams(FOnOrganizationListResponse, TSharedPtr<FOrganizationListResponse>, bool);

class RPMNEXTGENEDITOR_API FDeveloperAccountApi : public FWebApiWithAuth
{
public:	
	FDeveloperAccountApi(const TSharedPtr<IAuthenticationStrategy>& InAuthenticationStrategy);
	void ListApplicationsAsync(TSharedPtr<FApplicationListRequest> Request, FOnApplicationListResponse OnComplete);
	void ListOrganizationsAsync(TSharedPtr<FOrganizationListRequest> Request, FOnOrganizationListResponse OnComplete);

private:
	FString ApiBaseUrl;
	static FString BuildQueryString(const TMap<FString, FString>& Params);
};
