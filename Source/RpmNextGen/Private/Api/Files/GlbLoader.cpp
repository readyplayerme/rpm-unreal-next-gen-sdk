#include "Api/Files/GlbLoader.h"
#include "RpmNextGen.h"
#include "glTFRuntime/Public/glTFRuntimeFunctionLibrary.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "Cache/FileWriter.h"

FGlbLoader::FGlbLoader() : GltfConfig(new FglTFRuntimeConfig()), FileWriter(new FFileWriter()) 
{
    GltfConfig->TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
    OnFileRequestComplete.BindRaw( this, &FGlbLoader::HandleFileRequestComplete);
}

FGlbLoader::FGlbLoader(FglTFRuntimeConfig* Config) : FGlbLoader()
{
    if (Config)
    {
        delete GltfConfig;
        GltfConfig = Config;
    }
}

FGlbLoader::~FGlbLoader()
{
    delete GltfConfig;  
    delete FileWriter;
}

void FGlbLoader::HandleFileRequestComplete(TArray<uint8>* Data, const FString& FileName, const FString& AssetType)
{
    UglTFRuntimeAsset* GltfAsset = nullptr;
    if (Data)
    {
        UE_LOG(LogReadyPlayerMe, Warning, TEXT("Glb url request complete"));
        if(OnGLtfAssetLoaded.IsBound())
        {
            GltfAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(*Data, *GltfConfig);
            OnGLtfAssetLoaded.ExecuteIfBound(GltfAsset, AssetType);
            UE_LOG(LogReadyPlayerMe, Warning, TEXT("Glb Loaded of type %s"), *AssetType);
        }
        return;
    }
    UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load GLB from URL"));
    OnGLtfAssetLoaded.ExecuteIfBound(GltfAsset, AssetType);
}
