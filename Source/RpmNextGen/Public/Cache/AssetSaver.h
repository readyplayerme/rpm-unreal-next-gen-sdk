#pragma once

#include "CoreMinimal.h"

struct FStoredAsset;
class FHttpModule;
class IHttpResponse;
class IHttpRequest;
struct FAsset;

DECLARE_DELEGATE_OneParam(FOnAssetSavedToCache, bool);

class RPMNEXTGEN_API FAssetSaver : public TSharedFromThis<FAssetSaver>
{
	
public:
	FAssetSaver();
	virtual ~FAssetSaver();
	void LoadSaveAssetToCache(const FString& BaseModelId, const FAsset* Asset);
	void LoadAndSaveImage(const FStoredAsset& StoredAsset);
	void LoadAndSaveGlb(const FStoredAsset& StoredAsset);
	void OnAssetLoaded(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, const FStoredAsset* StoredAsset);
	void SaveToFile(const FString& FilePath, const TArray<uint8>& Data);

	FOnAssetSavedToCache OnAssetSaved;
private:
	bool bIsImageLoaded;
	bool bIsGlbLoaded;
	static const FString CacheFolderPath;
	FHttpModule* Http;
};
