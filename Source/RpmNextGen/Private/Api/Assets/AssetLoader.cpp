#include "Api/Assets/AssetLoader.h"
#include "HttpModule.h"
#include "glTFRuntime/Public/glTFRuntimeFunctionLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"

FAssetLoader::FAssetLoader()
{
    GltfConfig = new FglTFRuntimeConfig();
    GltfConfig->TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
    DownloadDirectory = FPaths::ProjectContentDir() / TEXT("ReadyPlayerMe/");
    
    // Ensure the directory exists
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*DownloadDirectory))
    {
        PlatformFile.CreateDirectory(*DownloadDirectory);
    }
}

FAssetLoader::FAssetLoader(FglTFRuntimeConfig* Config) : GltfConfig(Config)
{
}

FAssetLoader::~FAssetLoader()
{
}

void FAssetLoader::LoadGLBFromURL(const FString& URL)
{
    // TODO replace this with use of WebApi class
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->OnProcessRequestComplete().BindRaw(this, &FAssetLoader::OnDownloadComplete);
    HttpRequest->SetURL(URL);
    HttpRequest->SetVerb(TEXT("GET"));
    HttpRequest->ProcessRequest();
}

void FAssetLoader::OnDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    
    if (bWasSuccessful && Response.IsValid())
    {
        const TArray<uint8> Content = Response->GetContent();
        OnRequestDataReceived.ExecuteIfBound(Content, true);

        const FString FileName = FPaths::GetCleanFilename(Request->GetURL());
        const FString FilePath = DownloadDirectory / FileName;
        
        if (FFileHelper::SaveArrayToFile(Response->GetContent(), *FilePath))
        {
            UE_LOG(LogTemp, Log, TEXT("Downloaded GLB file to %s"), *FilePath);
            UglTFRuntimeAsset* gltfAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(Content, *GltfConfig);
            OnGLtfAssetLoaded.ExecuteIfBound(FilePath, gltfAsset, true);
            return;
        }
        
        UE_LOG(LogTemp, Error, TEXT("Failed to save GLB file to %s"), *FilePath);
        OnGLtfAssetLoaded.ExecuteIfBound(FString(), nullptr, false);
        return;
    }
    UE_LOG(LogTemp, Error, TEXT("Failed to download GLB file from URL"));
    OnRequestDataReceived.ExecuteIfBound(TArray<uint8>(), false);
    OnGLtfAssetLoaded.ExecuteIfBound(FString(), nullptr, false);
}
