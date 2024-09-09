#pragma once
#include "Api/Assets/Models/AssetListResponse.h"

class FTaskManager;
struct FAssetSaveData;
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

class RPMNEXTGEN_API FCacheGenerator : public TSharedFromThis<FCacheGenerator>
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
	void LoadAndStoreAssetGlb(const FString& BaseModelId, const FAsset* Asset);
	void LoadAndStoreAssetIcon(const FString& BaseModelId, const FAsset* Asset);

protected:
	void FetchBaseModels() const;
	void FetchAssetTypes() const;
	void FetchAssetsForBaseModel(const FString& BaseModelID, const FString& AssetType) const;

	virtual void OnDownloadRemoteCacheComplete(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful);
	void OnAssetSaved(const FAsset& Asset, const TArray<uint8>& Data);
	TUniquePtr<FAssetApi> AssetApi;
	TArray<FAsset> BaseModelAssets;
	TArray<FString> AssetTypes;
	TMap<FString, TArray<FAsset>> RefittedAssetMapByBaseModelId; 
	int32 CurrentBaseModelIndex;
private:
	void OnListAssetsResponse(const FAssetListResponse& AssetListResponse, bool bWasSuccessful);
	void FetchAssetsForEachBaseModel();
	void OnListAssetTypesResponse(const FAssetTypeListResponse& AssetTypeListResponse, bool bWasSuccessful);
	static const FString ZipFileName;
	int MaxItemsPerCategory;
	int RequiredRefittedAssetRequests = 0;
	int RefittedAssetRequestsCompleted = 0;
	int RequiredAssetDownloadRequest = 0;
	int AssetDownloadRequestsCompleted = 0;
	int AssetListResponseCounter = 0;
	FHttpModule* Http;
};
