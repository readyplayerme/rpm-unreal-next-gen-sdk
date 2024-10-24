#pragma once
#include "Api/Common/ApiRequestStrategy.h"
#include "Api/Common/WebApiWithAuth.h"
#include "Models/AssetTypeListResponse.h"

struct FApiRequest;
struct FAssetTypeListRequest;
struct FAssetListRequest;
struct FAssetListResponse;

//TODO cleanup if not needed
// DECLARE_DELEGATE_TwoParams(FOnListAssetsResponse, const FAssetListResponse&, bool);
// DECLARE_DELEGATE_TwoParams(FOnListAssetTypeResponse, const FAssetTypeListResponse&, bool);

class RPMNEXTGEN_API FAssetApi :  public FWebApiWithAuth
{
public:
	static const FString BaseModelType;

	//TODO cleanup if not needed
	// FOnListAssetsResponse OnListAssetsResponse;
	// FOnListAssetTypeResponse OnListAssetTypeResponse;

	
	FAssetApi();
	FAssetApi(EApiRequestStrategy InApiRequestStrategy);
	virtual ~FAssetApi() override;
	
	void Initialize();
	void ListAssetsAsync(TSharedPtr<FAssetListRequest> Request, TFunction<void(TSharedPtr<FAssetListResponse>, bool)> OnComplete);
	void ListAssetTypesAsync(TSharedPtr<FAssetTypeListRequest> Request, TFunction<void(TSharedPtr<FAssetTypeListResponse>, bool)> OnComplete);
protected:
	EApiRequestStrategy ApiRequestStrategy;
	
private:
	FString ApiBaseUrl;
	bool bIsInitialized = false;
	
	void LoadAssetsFromCache(TMap<FString, FString> QueryParams, TFunction<void(TSharedPtr<FAssetListResponse>, bool)> OnComplete);
	void LoadAssetTypesFromCache(TFunction<void(TSharedPtr<FAssetTypeListResponse>, bool)> OnComplete);

	TArray<FString> ExtractQueryValues(const FString& QueryString, const FString& Key);
};
