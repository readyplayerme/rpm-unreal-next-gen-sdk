#pragma once
#include "Api/Common/ApiRequestStrategy.h"
#include "Api/Common/WebApiWithAuth.h"
#include "Models/AssetTypeListResponse.h"

struct FApiRequest;
struct FAssetTypeListRequest;
struct FAssetListRequest;
struct FAssetListResponse;

DECLARE_DELEGATE_TwoParams(FOnListAssetsResponse, const FAssetListResponse&, bool);
DECLARE_DELEGATE_TwoParams(FOnListAssetTypeResponse, const FAssetTypeListResponse&, bool);

class RPMNEXTGEN_API FAssetApi :  public FWebApiWithAuth
{
public:
	static const FString BaseModelType;
	
	FOnListAssetsResponse OnListAssetsResponse;
	FOnListAssetTypeResponse OnListAssetTypeResponse;
	
	FAssetApi();
	FAssetApi(EApiRequestStrategy InApiRequestStrategy);
	
	void Initialize();
	void ListAssetsAsync(const FAssetListRequest& Request);
	void ListAssetTypesAsync(const FAssetTypeListRequest& Request);
protected:
	EApiRequestStrategy ApiRequestStrategy;
	
private:
	FString ApiBaseUrl;
	bool bIsInitialized = false;
	//void HandleAssetListResponse(const FApiRequest& ApiRequest, FHttpResponsePtr Response, bool bWasSuccessful);
	void HandleAssetResponse(const FApiRequest& ApiRequest, FHttpResponsePtr Response, bool bWasSuccessful);
	//HandleAssetTypeListResponse(const FApiRequest& ApiRequest, FHttpResponsePtr Response, bool bWasSuccessful);
	
	void LoadAssetsFromCache(TMap<FString, FString> QueryParams);
	void LoadAssetTypesFromCache();

	TArray<FString> ExtractQueryValues(const FString& QueryString, const FString& Key);
};
