#pragma once

#include "CoreMinimal.h"
#include "Api/Auth/ApiRequest.h"
#include "Api/Common/Models/ApiResponse.h"
#include "Api/Common/WebApiWithAuth.h"
#include "DeveloperAccountApi.generated.h"

USTRUCT(BlueprintType)
struct RPMNEXTGENEDITOR_API FApplicationListRequest : public FApiRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "API")
	TMap<FString, FString> Params;
};

USTRUCT(BlueprintType)
struct RPMNEXTGENEDITOR_API FOrganizationListRequest : public FApiRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "API")
	TMap<FString, FString> Params;
};

USTRUCT(BlueprintType)
struct RPMNEXTGENEDITOR_API FApplication
{
	GENERATED_BODY()
	UPROPERTY(meta = (JsonName = "id"))
	FString Id;
	UPROPERTY(meta=(Jsonname = "name" ))
	FString Name;	
};

USTRUCT(BlueprintType)
struct RPMNEXTGENEDITOR_API FApplicationListResponse : public FApiResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "API", meta = (JsonName = "data"))
	TArray<FApplication> Data;
};

USTRUCT(BlueprintType)
struct RPMNEXTGENEDITOR_API FOrganization
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (JsonName = "id"))
	FString Id;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (JsonName = "name"))
	FString Name;	
};

USTRUCT(BlueprintType)
struct RPMNEXTGENEDITOR_API FOrganizationListResponse : public FApiResponse
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "API", meta = ( JsonName = "data"))
	TArray<FOrganization> Data;
};

DECLARE_DELEGATE_TwoParams(FOnApplicationListResponse, const FApplicationListResponse&, bool);
DECLARE_DELEGATE_TwoParams(FOnOrganizationListResponse, const FOrganizationListResponse&, bool);


class RPMNEXTGENEDITOR_API FDeveloperAccountApi : public FWebApiWithAuth
{
public:
	FDeveloperAccountApi(IAuthenticationStrategy* InAuthenticationStrategy);
	void ListApplicationsAsync(const FApplicationListRequest& Request);
	void ListOrganizationsAsync(const FOrganizationListRequest& Request);
	
	FOnApplicationListResponse OnApplicationListResponse;
	FOnOrganizationListResponse OnOrganizationResponse;
private:
	static FString BuildQueryString(const TMap<FString, FString>& Params);
	void HandleOrgListResponse(FString Data, bool bWasSuccessful);
	void HandleAppListResponse(FString Data, bool bWasSuccessful);

	FString ApiBaseUrl;
};
