﻿#pragma once

#include "CoreMinimal.h"
#include "Models/Asset.h"

struct FAssetLoadingContext;
struct FCachedAssetData;
class IHttpResponse;
class FHttpModule;
struct FAsset;

class RPMNEXTGEN_API FAssetGlbLoader : public TSharedFromThis<FAssetGlbLoader>
{
public:
	DECLARE_DELEGATE_TwoParams(FOnGlbLoaded, const FAsset&, const TArray<uint8>&);

	FAssetGlbLoader();
	virtual ~FAssetGlbLoader();
	void LoadGlb(const FAsset& Asset, const FString& BaseModelId, bool bStoreInCache);

	FOnGlbLoaded OnGlbLoaded;
    
private:
	void LoadGlb(TSharedRef<FAssetLoadingContext> Context);
	void GlbLoaded(TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, const TSharedRef<FAssetLoadingContext>& Context);

	FHttpModule* Http;
	
};