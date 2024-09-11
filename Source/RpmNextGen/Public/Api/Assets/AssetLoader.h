#pragma once

#include "CoreMinimal.h"
#include "Models/Asset.h"

struct FCachedAssetData;
class IHttpResponse;
class FHttpModule;
struct FAsset;

struct FAssetLoadingContext
{
	FAsset Asset;
	FString BaseModelId;
	TArray<uint8> Data;
	bool bStoreInCache;
	bool bIsGLb;
	FAssetLoadingContext(const FAsset& InAsset, const FString& InBaseModelId, bool bInStoreInCache)
		: Asset(InAsset), BaseModelId(InBaseModelId), bStoreInCache(bInStoreInCache), bIsGLb(false)
	{
	}
};

class RPMNEXTGEN_API FAssetLoader : public TSharedFromThis<FAssetLoader>
{
public:

	DECLARE_DELEGATE_TwoParams(FOnAssetGlbLoaded, const FAsset&, const TArray<uint8>&);
	DECLARE_DELEGATE_TwoParams(FOnAsseImageLoaded, const FAsset&, const TArray<uint8>&);
	
	FAssetLoader();
	virtual ~FAssetLoader();
	void LoadAssetIcon(const FAsset& Asset, const FString& BaseModelId, bool bStoreInCache);
	void LoadAssetGlb(const FAsset& Asset, const FString& BaseModelId, bool bStoreInCache);
	FOnAssetGlbLoaded OnAssetGlbLoaded;
	FOnAsseImageLoaded OnAssetIconLoaded;
private:
	void LoadAssetGlb(TSharedRef<FAssetLoadingContext> Context);
	void LoadAssetIcon(TSharedRef<FAssetLoadingContext> Context);
	void AssetGlbLoaded(TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, const TSharedRef<FAssetLoadingContext>& Context);
	void AssetIconLoaded(TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, const TSharedRef<FAssetLoadingContext>& Context);

	FHttpModule* Http;
};
