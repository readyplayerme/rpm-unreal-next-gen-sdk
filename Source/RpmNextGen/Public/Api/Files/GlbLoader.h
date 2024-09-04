#pragma once

#include "CoreMinimal.h"
#include "FileApi.h"
#include "HAL/PlatformFilemanager.h"
#include "Interfaces/IHttpRequest.h"

struct FglTFRuntimeConfig;
class UglTFRuntimeAsset;

DECLARE_DELEGATE_TwoParams(FOnGlbDataReceived, TArray<uint8>, bool);
DECLARE_DELEGATE_TwoParams(FOnGlbSaved, FString, bool);
DECLARE_DELEGATE_TwoParams(FOnGlbDownloaded, UglTFRuntimeAsset*, bool);


class RPMNEXTGEN_API FGlbLoader : public FFileApi
{
public:
	FGlbLoader();
	FGlbLoader(FglTFRuntimeConfig* Config);
	virtual ~FGlbLoader() override;

	FOnGlbDataReceived OnRequestDataReceived;
	FOnGlbDownloaded OnGLtfAssetLoaded;
	FOnGlbSaved OnAssetSaved;
	bool bSaveToDisk = false;

protected:
	FglTFRuntimeConfig* GltfConfig;
	virtual void FileRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) override;
	FString DownloadDirectory;
};
