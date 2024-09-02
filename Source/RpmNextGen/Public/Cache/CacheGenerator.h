#pragma once
#include "Api/Assets/Models/AssetListResponse.h"

struct FAssetTypeListResponse;
class FAssetApi;
struct FAsset;
class IHttpResponse;
class IHttpRequest;
class FHttpModule;

DECLARE_DELEGATE_OneParam(FOnGenerateLocalCache, bool);
DECLARE_DELEGATE_OneParam(FOnDownloadRemoteCache, bool);

class RPMNEXTGEN_API FCacheGenerator
{
public:
	virtual ~FCacheGenerator() = default;
	FCacheGenerator();
	void DownloadRemoteCacheFromUrl(const FString& Url);
	void GenerateLocalCache(int InItemsPerCategory);
	void ExtractCache();
	
	FOnDownloadRemoteCache OnDownloadRemoteCacheDelegate;
	FOnGenerateLocalCache OnGenerateLocalCacheDelegate;
protected:
	void FetchBaseModels();
	void FetchAssetTypes();
	void FetchAssetsForBaseModel(const FString& BaseModelID, const FString& AssetType);

	virtual void OnRequestCacheAssetsComplete(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful);
	virtual void OnDownloadRemoteCacheComplete(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful);
	
	TUniquePtr<FAssetApi> AssetApi;

	TArray<FAsset> BaseModelAssets;
	TArray<FString> AssetTypes;
	TMap<FString, TArray<FAsset>> BaseModelAssetsMap; 
	int32 CurrentBaseModelIndex;
	
private:
	void ProcessNextRequest();
	
	void OnListAssetsResponse(const FAssetListResponse& AssetListResponse, bool bWasSuccessful);
	void FetchAssetsForEachBaseModel();
	void OnListAssetTypesResponse(const FAssetTypeListResponse& AssetTypeListResponse, bool bWasSuccessful);
	static const FString CacheFolderPath;
	static const FString ZipFileName;
	int ItemsPerCategory;

	FHttpModule* Http;
};
