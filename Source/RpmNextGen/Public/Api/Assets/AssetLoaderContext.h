#pragma once

#include "CoreMinimal.h"
#include "Models/Asset.h"

struct FAssetLoadingContext
{
	FAsset Asset;
	FString BaseModelId;
	TArray<uint8> Data;
	bool bStoreInCache;
	FAssetLoadingContext(const FAsset& InAsset, const FString& InBaseModelId, bool bInStoreInCache)
		: Asset(InAsset), BaseModelId(InBaseModelId), bStoreInCache(bInStoreInCache)
	{
	}
};
