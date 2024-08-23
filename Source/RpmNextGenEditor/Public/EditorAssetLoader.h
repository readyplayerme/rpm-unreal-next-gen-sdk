#pragma once

#include "CoreMinimal.h"
#include "TransientObjectSaverLibrary.h"
#include "Api/Assets/AssetLoader.h"
#include "HAL/PlatformFilemanager.h"

class RPMNEXTGENEDITOR_API FEditorAssetLoader : public FAssetLoader
{
public:
	void OnAssetDownloadComplete(FString FilePath, UglTFRuntimeAsset* gltfAsset, bool bWasSuccessful,
	                             FString LoadedAssetId);
	FEditorAssetLoader();
	virtual ~FEditorAssetLoader() override;

	void LoadAssetToWorldAsURpmActor(UglTFRuntimeAsset* gltfAsset);
	void LoadAssetToWorldAsURpmActor(USkeletalMesh* gltfAsset);
	void LoadGLBFromURLWithId(const FString& URL, const FString AssetId);
	USkeletalMesh* SaveAsUAsset(UglTFRuntimeAsset* GltfAsset, const FString& LoadedAssetId) const;
	USkeleton* SkeletonToCopy;

private:
	void LoadAssetToWorld(USkeletalMesh* SkeletalMesh, UglTFRuntimeAsset* gltfAsset);
};
