#pragma once

#include "CoreMinimal.h"
#include "Api/Common/WebApi.h"
#include "HAL/PlatformFilemanager.h"
#include "Interfaces/IHttpRequest.h"

struct FglTFRuntimeConfig;
class UglTFRuntimeAsset;

DECLARE_DELEGATE_TwoParams(FOnAssetDataReceived, TArray<uint8>, bool);
DECLARE_DELEGATE_TwoParams(FOnAssetSaved, FString, bool);
DECLARE_DELEGATE_TwoParams(FOnAssetDownloaded, UglTFRuntimeAsset*, bool);


class RPMNEXTGEN_API FAssetLoader : public FWebApi
{
public:
	FAssetLoader();
	FAssetLoader(FglTFRuntimeConfig* Config);
	virtual ~FAssetLoader() override;

	void LoadGLBFromURL(const FString& URL);

	FOnAssetDataReceived OnRequestDataReceived;
	FOnAssetDownloaded OnGLtfAssetLoaded;
	FOnAssetSaved OnAssetSaved;
	bool bSaveToDisk = false;

protected:
	FglTFRuntimeConfig* GltfConfig;
	void virtual OnLoadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	FString DownloadDirectory;
};
