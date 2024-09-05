#pragma once

#include "CoreMinimal.h"
#include "Models/Asset.h"

struct FAssetSaveData;
class IHttpResponse;
class FHttpModule;
struct FAsset;

struct FAssetLoadingContext
{
	FAsset Asset;
	FString BaseModelId;
	TArray<uint8> ImageData;
	TArray<uint8> GlbData;
	bool bStoreInCache;
	FAssetLoadingContext(const FAsset& InAsset, const FString& InBaseModelId, bool bInStoreInCache)
		: Asset(InAsset), BaseModelId(InBaseModelId), bStoreInCache(bInStoreInCache) {}
};



class RPMNEXTGEN_API FAssetLoader : public TSharedFromThis<FAssetLoader>
{
public:

	DECLARE_DELEGATE_OneParam(FOnAssetGlbLoaded, const TArray<uint8>&);
	DECLARE_DELEGATE_OneParam(FOnAsseImageLoaded, const TArray<uint8>&);
	DECLARE_DELEGATE_OneParam(FOnAssetSaved, const FAssetSaveData&);
	
	FAssetLoader();
	virtual ~FAssetLoader();
	void LoadAsset(const FAsset& Asset, const FString& BaseModelId, bool bStoreInCache);
	FOnAssetGlbLoaded OnAssetGlbLoaded;
	FOnAsseImageLoaded OnAssetImageLoaded;
	FOnAssetSaved OnAssetSaved;
private:
	void LoadAssetModel(TSharedRef<FAssetLoadingContext> Context);
	void LoadAssetImage(TSharedRef<FAssetLoadingContext> Context);
	void AssetModelLoaded(TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, const TSharedRef<FAssetLoadingContext>& Context);
	void AssetImageLoaded(TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, const TSharedRef<FAssetLoadingContext>& Context);

	FHttpModule* Http;
};
