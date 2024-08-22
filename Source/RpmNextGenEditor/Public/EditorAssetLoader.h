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

	void LoadGltfAssetToWorld(UglTFRuntimeAsset* gltfAsset);
	void LoadGLBFromURLWithId(const FString& URL, const FString AssetId);
	void SaveAsUAsset(UglTFRuntimeAsset* gltfAsset, FString Path);
	USkeleton* SkeletonToCopy;
};
