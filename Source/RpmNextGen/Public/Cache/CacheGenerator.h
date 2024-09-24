#pragma once
#include "Api/Assets/Models/AssetListResponse.h"

struct FAssetListRequest;
class FTaskManager;
struct FCachedAssetData;
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

	void Reset();

protected:
	void FetchBaseModels() const;
	void FetchAssetTypes() const;

	virtual void OnDownloadRemoteCacheComplete(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful);
	UFUNCTION()
	void OnAssetGlbSaved(const FAsset& Asset, const TArray<uint8>& Data);
	UFUNCTION()
	void OnAssetIconSaved(const FAsset& Asset, const TArray<uint8>& Data);
	void AddFolderToNonAssetDirectory() const;
	void FetchNextRefittedAsset();
	
	TUniquePtr<FAssetApi> AssetApi;
	TArray<FString> AssetTypes;
	TMap<FString, TArray<FAsset>> AssetMapByBaseModelId;
	TArray<FAssetListRequest> AssetListRequests;
	int32 CurrentBaseModelIndex;
private:
	void OnListAssetsResponse(const FAssetListResponse& AssetListResponse, bool bWasSuccessful);
	void StartFetchingRefittedAssets();
	void OnListAssetTypesResponse(const FAssetTypeListResponse& AssetTypeListResponse, bool bWasSuccessful);
	static const FString ZipFileName;
	int MaxItemsPerCategory;
	int RefittedAssetRequestsCompleted = 0;
	int RequiredAssetDownloadRequest = 0;
	int NumberOfAssetsSaved = 0;
	FHttpModule* Http;
};
