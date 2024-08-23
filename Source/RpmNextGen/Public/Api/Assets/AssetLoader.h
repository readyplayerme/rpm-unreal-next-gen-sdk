#pragma once

#include "CoreMinimal.h"
#include "Api/Common/WebApi.h"
#include "HAL/PlatformFilemanager.h"
#include "Interfaces/IHttpRequest.h"

struct FglTFRuntimeConfig;
class UglTFRuntimeAsset;

DECLARE_DELEGATE_TwoParams(FOnAssetDataReceived, TArray<uint8>, bool);
DECLARE_DELEGATE_ThreeParams(FOnAssetDownloaded, FString, UglTFRuntimeAsset*, bool);

class RPMNEXTGEN_API FAssetLoader : public FWebApi
{
public:
	FAssetLoader();
	FAssetLoader(FglTFRuntimeConfig* Config);
	virtual ~FAssetLoader() override;

	void LoadGLBFromURL(const FString& URL);

	FOnAssetDataReceived OnAssetDataReceived;
	FOnAssetDownloaded OnAssetDownloaded;

protected:
	FglTFRuntimeConfig* GltfConfig;
	void virtual OnDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	FString DownloadDirectory;
};
