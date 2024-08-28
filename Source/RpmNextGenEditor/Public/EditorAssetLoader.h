#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/AssetLoader.h"
#include "HAL/PlatformFilemanager.h"

class RPMNEXTGENEDITOR_API FEditorAssetLoader : public FAssetLoader
{
public:
	void OnAssetLoadComplete(UglTFRuntimeAsset* gltfAsset, bool bWasSuccessful,
	                             FString LoadedAssetId);
	FEditorAssetLoader();
	virtual ~FEditorAssetLoader() override;

	void LoadAssetToWorldAsURpmActor(UglTFRuntimeAsset* gltfAsset, FString AssetId = "");
	void LoadGLBFromURLWithId(const FString& URL, const FString AssetId);
	USkeletalMesh* SaveAsUAsset(UglTFRuntimeAsset* GltfAsset, const FString& LoadedAssetId) const;
	USkeleton* SkeletonToCopy;

private:
	void LoadAssetToWorld(FString AssetId, UglTFRuntimeAsset* gltfAsset);
};
