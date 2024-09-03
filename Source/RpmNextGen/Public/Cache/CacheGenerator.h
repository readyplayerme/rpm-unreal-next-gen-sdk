#pragma once
#include "Api/Assets/Models/AssetListResponse.h"

class FAssetSaver;
struct FAssetTypeListResponse;
class FAssetApi;
struct FAsset;
class IHttpResponse;
class IHttpRequest;
class FHttpModule;

DECLARE_DELEGATE_OneParam(FOnCacheDataLoaded, bool);
DECLARE_DELEGATE_OneParam(FOnLocalCacheGenerated, bool);
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
	FOnCacheDataLoaded OnCacheDataLoaded;
	FOnLocalCacheGenerated OnLocalCacheGenerated;
	
	void LoadAndStoreAssets();
	void LoadAndStoreAssetFromUrl(const FString& BaseModelId, const FAsset* Asset);

protected:
	void FetchBaseModels();
	void FetchAssetTypes();
	void FetchAssetsForBaseModel(const FString& BaseModelID, const FString& AssetType);

	virtual void OnDownloadRemoteCacheComplete(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful);
	virtual void OnAssetDataLoaded(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, const FString& FilePath);
	void OnAssetSaved(bool bWasSuccessful);
	TUniquePtr<FAssetApi> AssetApi;
	TArray<FAsset> BaseModelAssets;
	TArray<FString> AssetTypes;
	TMap<FString, TArray<FAsset>> BaseModelAssetsMap; 
	int32 CurrentBaseModelIndex;
private:
	void OnListAssetsResponse(const FAssetListResponse& AssetListResponse, bool bWasSuccessful);
	void FetchAssetsForEachBaseModel();
	void OnListAssetTypesResponse(const FAssetTypeListResponse& AssetTypeListResponse, bool bWasSuccessful);
	static const FString CacheFolderPath;
	static const FString ZipFileName;
	int MaxItemsPerCategory;
	int RequiredRefittedAssetRequests = 0;
	int RefittedAssetRequestsCompleted = 0;
	int RequiredAssetDownloadRequest = 0;
	int AssetDownloadRequestsCompleted = 0;
	FHttpModule* Http;
};
