#include "Api/Assets/AssetLoader.h"
#include "HttpModule.h"
#include "glTFRuntime/Public/glTFRuntimeFunctionLibrary.h"
#include "Engine/World.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"

FAssetLoader::FAssetLoader()
{
    GltfConfig = new FglTFRuntimeConfig();
    GltfConfig->TransformBaseType = EglTFRuntimeTransformBaseType::YForward;
    
    // Set the download directory within the project
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
        TArray<uint8> Content = Response->GetContent();
        OnAssetDataReceived.ExecuteIfBound(true, Content);
        
        FString FileName = FPaths::GetCleanFilename(Request->GetURL());
        FString FilePath = DownloadDirectory / FileName;
        
        // Write the file to disk
        if (FFileHelper::SaveArrayToFile(Response->GetContent(), *FilePath))
        {
            UE_LOG(LogTemp, Log, TEXT("Downloaded GLB file to %s"), *FilePath);
            UglTFRuntimeAsset* gltfAsset = UglTFRuntimeFunctionLibrary::glTFLoadAssetFromData(Content, *GltfConfig);
            OnAssetDownloaded.ExecuteIfBound(true, FilePath, gltfAsset);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to save GLB file to %s"), *FilePath);
        }
        
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to download GLB file from URL"));
    }
    OnAssetDataReceived.ExecuteIfBound(false, TArray<uint8>());
    OnAssetDownloaded.ExecuteIfBound(false, FString(), nullptr);
}


