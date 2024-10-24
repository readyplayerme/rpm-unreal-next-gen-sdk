#pragma once
#include "Api/Common/ApiRequestStrategy.h"
#include "Api/Common/WebApiWithAuth.h"
#include "Models/AssetTypeListResponse.h"

struct FApiRequest;
struct FAssetTypeListRequest;
struct FAssetListRequest;
struct FAssetListResponse;

DECLARE_DELEGATE_TwoParams(FOnListAssetsResponse, TSharedPtr<FAssetListResponse>, bool);
DECLARE_DELEGATE_TwoParams(FOnListAssetTypesResponse, TSharedPtr<FAssetTypeListResponse>, bool);

class RPMNEXTGEN_API FAssetApi :  public FWebApiWithAuth
{
public:
	static const FString BaseModelType;
	
	FAssetApi();
	FAssetApi(EApiRequestStrategy InApiRequestStrategy);
	virtual ~FAssetApi() override;
	
	void Initialize();
	void ListAssetsAsync(TSharedPtr<FAssetListRequest> Request, FOnListAssetsResponse OnComplete);
	void ListAssetTypesAsync(TSharedPtr<FAssetTypeListRequest> Request, FOnListAssetTypesResponse OnComplete);
protected:
	EApiRequestStrategy ApiRequestStrategy;
	
private:
	FString ApiBaseUrl;
	bool bIsInitialized = false;
	
	void LoadAssetsFromCache(TMap<FString, FString> QueryParams, FOnListAssetsResponse OnComplete);
	void LoadAssetTypesFromCache(FOnListAssetTypesResponse OnComplete);

	TArray<FString> ExtractQueryValues(const FString& QueryString, const FString& Key);
};
