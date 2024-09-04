#include "Api/Files/GlbLoader.h"
#include "RpmNextGen.h"
#include "glTFRuntime/Public/glTFRuntimeFunctionLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"

FGlbLoader::FGlbLoader() : GltfConfig(nullptr)
{
    GltfConfig = new FglTFRuntimeConfig();
    GltfConfig->TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
    DownloadDirectory = FPaths::ProjectContentDir() / TEXT("ReadyPlayerMe/AvatarCache");
    
    // Ensure the directory exists
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*DownloadDirectory))
    {
        PlatformFile.CreateDirectory(*DownloadDirectory);
    }
}

FGlbLoader::FGlbLoader(FglTFRuntimeConfig* Config) : GltfConfig(Config)
{
    FGlbLoader();
}

FGlbLoader::~FGlbLoader()
{
}

void FGlbLoader::FileRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    TArray<uint8> Content = TArray<uint8>();
    UglTFRuntimeAsset* gltfAsset = nullptr;
    bool AssetFileSaved = false;
    if (bWasSuccessful && Response.IsValid())
    {
        Content = Response->GetContent();

        const FString FileName = FPaths::GetCleanFilename(Request->GetURL());
        const FString FilePath = DownloadDirectory / FileName;

        if(bSaveToDisk && FFileHelper::SaveArrayToFile(Content, *FilePath))
        {
            UE_LOG(LogReadyPlayerMe, Log, TEXT("Downloaded GLB file to %s"), *FilePath);
            AssetFileSaved = true;
        }
        if(OnGLtfAssetLoaded.IsBound())
        {
            gltfAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(Content, *GltfConfig);
        }
        OnRequestDataReceived.ExecuteIfBound(Content, Content.Num() > 0);
        OnGLtfAssetLoaded.ExecuteIfBound(gltfAsset, gltfAsset != nullptr);
        OnAssetSaved.ExecuteIfBound(FilePath, AssetFileSaved);
        return;
    }
    UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load GLB from URL"));
    OnRequestDataReceived.ExecuteIfBound(Content, Content.Num() > 0);
    OnGLtfAssetLoaded.ExecuteIfBound(gltfAsset, gltfAsset != nullptr);
    OnAssetSaved.ExecuteIfBound("", AssetFileSaved);
}
