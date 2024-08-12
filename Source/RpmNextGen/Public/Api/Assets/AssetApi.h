#pragma once
#include "Api/Common/WebApiWithAuth.h"

struct FAssetListRequest;
struct FAssetListResponse;

DECLARE_DELEGATE_TwoParams(FOnListAssetsResponse, const FAssetListResponse&, bool);
DECLARE_DELEGATE_TwoParams(FOnListAssetTypeResponse, const FAssetListResponse&, bool);

class RPMNEXTGEN_API FAssetApi : public FWebApiWithAuth
{
public:
	FAssetApi();
	void ListAssetsAsync(const FAssetListRequest& Request);
	FOnListAssetsResponse OnListAssetsResponse;
	FOnListAssetTypeResponse OnListAssetTypeResponse;
private:
	void HandleListAssetResponse(FString Response, bool bWasSuccessful);
	void HandleListAssetTypeResponse(FString Response, bool bWasSuccessful);
	
	FString ApiBaseUrl;
};
