#pragma once
#include "Api/Common/ApiRequestStrategy.h"
#include "Api/Common/WebApiWithAuth.h"
#include "Models/AssetTypeListResponse.h"

struct FAssetTypeListRequest;
struct FAssetListRequest;
struct FAssetListResponse;

DECLARE_DELEGATE_TwoParams(FOnListAssetsResponse, const FAssetListResponse&, bool);
DECLARE_DELEGATE_TwoParams(FOnListAssetTypeResponse, const FAssetTypeListResponse&, bool);

class RPMNEXTGEN_API FAssetApi 
{
public:
	static const FString BaseModelType;
	
	FOnListAssetsResponse OnListAssetsResponse;
	FOnListAssetTypeResponse OnListAssetTypeResponse;
	
	FAssetApi();
	FAssetApi(EApiRequestStrategy InApiRequestStrategy);
	void Initialize();

	void SetAuthenticationStrategy(IAuthenticationStrategy* InAuthStrategy);
	void ListAssetsAsync(const FAssetListRequest& Request);
	void ListAssetTypesAsync(const FAssetTypeListRequest& Request);
protected:
	EApiRequestStrategy ApiRequestStrategy;
	
private:
	FString ApiBaseUrl;
	TSharedPtr<FWebApiWithAuth> AssetListApi;
	TSharedPtr<FWebApiWithAuth> AssetTypeListApi;
	
	void HandleAssetListResponse(FString Response, bool bWasSuccessful);
	void HandleAssetTypeListResponse(FString Response, bool bWasSuccessful);
	
	void LoadAssetsFromCache();
	void LoadAssetTypesFromCache();
};
