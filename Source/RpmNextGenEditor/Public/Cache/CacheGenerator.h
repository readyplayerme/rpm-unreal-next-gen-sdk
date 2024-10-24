#pragma once
#include "Api/Assets/Models/AssetListResponse.h"
#include "Api/Assets/Models/AssetListRequest.h"

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

class RPMNEXTGENEDITOR_API FCacheGenerator : public TSharedFromThis<FCacheGenerator>
{
public:
	FOnDownloadRemoteCache OnDownloadRemoteCacheDelegate;
	FOnCacheDataLoaded OnCacheDataLoaded;
	FOnLocalCacheGenerated OnLocalCacheGenerated;
	
	FCacheGenerator();
	virtual ~FCacheGenerator();
	
	void DownloadRemoteCacheFromUrl(const FString& Url);
	void GenerateLocalCache(int InItemsPerCategory);
	void ExtractCache();
	void LoadAndStoreAssets();
	void LoadAndStoreAssetGlb(const FString& BaseModelId, const FAsset* Asset);
	void LoadAndStoreAssetIcon(const FString& BaseModelId, const FAsset* Asset);
	void Reset();

protected:
	TUniquePtr<FAssetApi> AssetApi;
	TArray<FString> AssetTypes;
	TMap<FString, TArray<FAsset>> AssetMapByBaseModelId;
	TArray<TSharedPtr<FAssetListRequest>> AssetListRequests;
	int32 CurrentBaseModelIndex;
	
	UFUNCTION()
	void OnAssetGlbSaved(const FAsset& Asset, const TArray<uint8>& Data);
	UFUNCTION()
	void OnAssetIconSaved(const FAsset& Asset, const TArray<uint8>& Data);

	void AddFolderToNonAssetDirectory() const;
	void FetchBaseModels();
	void FetchAssetTypes();
	void FetchNextRefittedAsset();
	
	virtual void OnDownloadRemoteCacheComplete(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful);
	
private:
	static const FString ZipFileName;

	FHttpModule* Http;
	
	int MaxItemsPerCategory;
	int RefittedAssetRequestsCompleted = 0;
	int RequiredAssetDownloadRequest = 0;
	int NumberOfAssetsSaved = 0;
	
	void StartFetchingRefittedAssets();
	void OnListBaseModelsComplete(TSharedPtr<FAssetListResponse> AssetListResponse, bool bWasSuccessful);
	void OnListAssetsComplete(TSharedPtr<FAssetListResponse> AssetListResponse, bool bWasSuccessful);
	void OnListAssetTypesComplete(TSharedPtr<FAssetTypeListResponse> AssetTypeListResponse, bool bWasSuccessful);
};
