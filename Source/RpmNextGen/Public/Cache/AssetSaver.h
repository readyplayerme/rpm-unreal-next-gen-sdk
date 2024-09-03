#pragma once

#include "CoreMinimal.h"

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
	void SaveAssetToCache(const FString& BaseModelId, const FAsset* Asset);
	void LoadAndSaveImage(const FString& Url, const FString& FilePath);
	void LoadAndSaveGlb(const FString& Url, const FString& FilePath);
	void OnAssetLoaded(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, const FString& FilePath);
	void SaveToFile(const FString& FilePath, const TArray<uint8>& Data);

	FOnAssetSavedToCache OnAssetSaved;
private:
	bool bIsImageLoaded;
	bool bIsGlbLoaded;
	static const FString CacheFolderPath;
	FHttpModule* Http;
};
