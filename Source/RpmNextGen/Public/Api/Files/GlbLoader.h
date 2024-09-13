#pragma once

#include "CoreMinimal.h"
#include "FileApi.h"
#include "HAL/PlatformFilemanager.h"

class FFileUtility;
struct FglTFRuntimeConfig;
class UglTFRuntimeAsset;

DECLARE_DELEGATE_TwoParams(FOnGlbDownloaded, UglTFRuntimeAsset*, const FString&);

class RPMNEXTGEN_API FGlbLoader : public FFileApi
{
public:
	FGlbLoader();
	FGlbLoader(FglTFRuntimeConfig* Config);
	
	virtual ~FGlbLoader() override;

	void SetConfig(FglTFRuntimeConfig* Config)
	{
		GltfConfig = Config;
	}

	FOnGlbDownloaded OnGLtfAssetLoaded;

protected:
	FglTFRuntimeConfig* GltfConfig;
	virtual void HandleFileRequestComplete(TArray<uint8>* Data, const FString& String, const FString& AssetType);
	FFileUtility* FileWriter;
	FString DownloadDirectory;
};
