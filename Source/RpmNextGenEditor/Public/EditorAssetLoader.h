#pragma once

#include "CoreMinimal.h"
#include "Api/Files/GlbLoader.h"
#include "HAL/PlatformFilemanager.h"

class FEditorAssetLoader : public FGlbLoader
{
public:
	FEditorAssetLoader();
	virtual ~FEditorAssetLoader() override;

	void LoadAssetToWorldAsURpmActor(UglTFRuntimeAsset* GltfAsset, FString AssetId = "");
	void LoadGlbFromURLWithId(const FString& URL, const FString AssetId);
	void OnAssetLoadComplete(UglTFRuntimeAsset* GltfAsset, bool bWasSuccessful, FString LoadedAssetId);
	
	USkeletalMesh* SaveAsUAsset(UglTFRuntimeAsset* GltfAsset, const FString& LoadedAssetId) const;
	USkeleton* SkeletonToCopy;

private:
	void LoadAssetToWorld(const FString& AssetId, UglTFRuntimeAsset* GltfAsset);
};
