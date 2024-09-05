#include "Cache/AssetSaver.h"
#include "HttpModule.h"
#include "RpmNextGen.h"
#include "Api/Assets/Models/Asset.h"
#include "Cache/AssetStorageManager.h"
#include "Interfaces/IHttpResponse.h"

const FString FAssetSaver::CacheFolderPath = FPaths::ProjectPersistentDownloadDir() / TEXT("ReadyPlayerMe/AssetCache");


FAssetSaver::FAssetSaver()
{
	Http = &FHttpModule::Get();
}

FAssetSaver::~FAssetSaver()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*CacheFolderPath))
	{
		PlatformFile.CreateDirectory(*CacheFolderPath);
	}
}

void FAssetSaver::LoadSaveAssetToCache(const FString& BaseModelId, const FAsset* Asset)
{
	const FString Path = CacheFolderPath / BaseModelId;
	FStoredAsset StoredAsset = FStoredAsset();
	StoredAsset.Asset = *Asset;
	StoredAsset.GlbFilePath = FString::Printf(TEXT("%s/%s.glb"), *Path, *Asset->Id);
	StoredAsset.IconFilePath = FString::Printf(TEXT("%s/%s.png"), *Path, *Asset->Id);

	// Start by loading and saving the GLB file, then chain the image loading
	LoadAndSaveGlb(StoredAsset);
}

void FAssetSaver::LoadAndSaveGlb(const FStoredAsset& StoredAsset)
{
	if (StoredAsset.Asset.GlbUrl.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Glb URL is empty for asset: %s assetname: %s"), *StoredAsset.Asset.Id, *StoredAsset.Asset.Name);
		// Directly load the image if GLB URL is empty
		LoadAndSaveImage(StoredAsset);
		return;
	}

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(StoredAsset.Asset.GlbUrl);
	Request->SetVerb(TEXT("GET"));

	// Capture TSharedPtr<FAssetSaver> in the lambda to ensure it is not destroyed prematurely
	TSharedPtr<FAssetSaver> ThisPtr = SharedThis(this);
	Request->OnProcessRequestComplete().BindLambda([ThisPtr, StoredAsset](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		ThisPtr->OnGlbLoaded(Request, Response, bWasSuccessful, &StoredAsset);
	});

	Request->ProcessRequest();
}

void FAssetSaver::OnGlbLoaded(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, const FStoredAsset* StoredAsset)
{
	if (bWasSuccessful && Response.IsValid())
	{
		SaveToFile(StoredAsset->GlbFilePath, Response->GetContent());
	}
	else
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load GLB from url: %s"), *Request->GetURL());
	}

	// After the GLB is loaded (or failed), load and save the image
	LoadAndSaveImage(*StoredAsset);
}

void FAssetSaver::LoadAndSaveImage(const FStoredAsset& StoredAsset)
{
	if (StoredAsset.Asset.IconUrl.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Icon URL is empty for asset: %s assetname: %s"), *StoredAsset.Asset.Id, *StoredAsset.Asset.Name);
		OnAssetSaved.ExecuteIfBound(false);  // If both are empty, we still execute the delegate
		return;
	}

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(StoredAsset.Asset.IconUrl);
	Request->SetVerb(TEXT("GET"));

	// Capture TSharedPtr<FAssetSaver> in the lambda to ensure it is not destroyed prematurely
	TSharedPtr<FAssetSaver> ThisPtr = SharedThis(this);
	Request->OnProcessRequestComplete().BindLambda([ThisPtr, StoredAsset](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		ThisPtr->OnImageLoaded(Request, Response, bWasSuccessful, &StoredAsset);
	});

	Request->ProcessRequest();
}

void FAssetSaver::OnImageLoaded(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, const FStoredAsset* StoredAsset)
{
	if (bWasSuccessful && Response.IsValid())
	{
		SaveToFile(StoredAsset->IconFilePath, Response->GetContent());
	}
	else
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load image from url: %s"), *Request->GetURL());
	}

	// After the image is loaded, the process is complete
	FAssetStorageManager::Get().TrackStoredAsset(*StoredAsset);

	UE_LOG(LogReadyPlayerMe, Log, TEXT("Asset saved to cache"));
	OnAssetSaved.ExecuteIfBound(true);
}


void FAssetSaver::SaveToFile(const FString& FilePath, const TArray<uint8>& Data)
{
	if (FFileHelper::SaveArrayToFile(Data, *FilePath))
	{
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Successfully saved asset in local cache to: %s"), *FilePath);
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to saved asset local cache to: %s"), *FilePath);
}
