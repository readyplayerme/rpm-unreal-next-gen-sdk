﻿#pragma once

#include "CoreMinimal.h"
#include "Api/Assets/AssetLoader.h"
#include "HAL/PlatformFilemanager.h"

class RPMNEXTGENEDITOR_API FEditorAssetLoader : public FAssetLoader
{
public:
	void OnAssetDownloadComplete(bool bArg, FString String, UglTFRuntimeAsset* UglTFRuntimeAsset);
	FEditorAssetLoader();
	virtual ~FEditorAssetLoader() override;
	
	void LoadGltfAssetToWorld(UglTFRuntimeAsset* gltfAsset);

	void SaveAsUAsset(UglTFRuntimeAsset* gltfAsset, FString Path);
	
};