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
	FOnGlbDownloaded OnGLtfAssetLoaded;
	
	FGlbLoader();
	FGlbLoader(FglTFRuntimeConfig* Config);
	
	virtual ~FGlbLoader() override;

	void SetConfig(FglTFRuntimeConfig* Config)
	{
		GltfConfig = Config;
	}

protected:
	FglTFRuntimeConfig* GltfConfig;
	FFileUtility* FileWriter;
	FString DownloadDirectory;
	
	UFUNCTION()
	virtual void HandleFileRequestComplete(const TArray<uint8>& Data, const FString& String);
};
