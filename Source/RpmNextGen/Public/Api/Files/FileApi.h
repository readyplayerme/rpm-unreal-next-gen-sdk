#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"

struct FAsset;
DECLARE_DELEGATE_TwoParams(FOnFileRequestComplete, TArray<uint8>*, const FString&);
DECLARE_DELEGATE_TwoParams(FOnAssetFileRequestComplete, TArray<uint8>*, const FAsset&);

class RPMNEXTGEN_API FFileApi : public TSharedFromThis<FFileApi>
{
public:
	FFileApi();
	virtual ~FFileApi();
	virtual void LoadFileFromUrl(const FString& URL);
	virtual void LoadAssetFileFromUrl(const FString& URL, FAsset Asset);
	virtual void FileRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	virtual void AssetFileRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FAsset Asset);
	static bool LoadFileFromPath(const FString& Path, TArray<uint8>& OutContent);

	FOnAssetFileRequestComplete OnAssetFileRequestComplete;
	FOnFileRequestComplete OnFileRequestComplete;
};
