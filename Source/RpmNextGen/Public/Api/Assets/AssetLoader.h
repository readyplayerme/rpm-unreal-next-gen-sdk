#pragma once

#include "CoreMinimal.h"

struct FAsset;

class RPMNEXTGEN_API FAssetLoader : public TSharedFromThis<FAssetLoader>
{
public:
	FAssetLoader() = default;
	virtual ~FAssetLoader() = default;
	void LoadAsset(FAsset* Asset, bool bStoreInCache);
	virtual void FileRequestComplete(TArray<uint8>* Data, FAsset* Asset);
private:
	void LoadAssetModel(FAsset* Asset, bool bStoreInCache);
	void LoadAssetImage(FAsset* Asset, bool bStoreInCache);
	bool bIsModelLoaded;
	bool bIsImageLoaded;
};
