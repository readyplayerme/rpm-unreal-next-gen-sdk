﻿#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/AssetGlbLoader.h"
#include "HAL/PlatformFilemanager.h"

struct FglTFRuntimeConfig;
class UglTFRuntimeAsset;

class FEditorAssetLoader : public FAssetGlbLoader
{
public:

	FEditorAssetLoader();
	virtual ~FEditorAssetLoader() override;

	void LoadAssetToWorldAsURpmActor(UglTFRuntimeAsset* GltfAsset, FString AssetId = "");
	void LoadBaseModelAsset(const FAsset& Asset);
	
	USkeletalMesh* SaveAsUAsset(UglTFRuntimeAsset* GltfAsset, const FString& LoadedAssetId) const;
	USkeleton* SkeletonToCopy;

private:
	void LoadAssetToWorld(const FString& AssetId, UglTFRuntimeAsset* GltfAsset);
	UFUNCTION()
	void HandleGlbLoaded(const FAsset& Asset, const TArray<unsigned char>& Data);
	FglTFRuntimeConfig* GltfConfig;
};
