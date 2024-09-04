#include "Cache/AssetSaver.h"
#include "HttpModule.h"
#include "RpmNextGen.h"
#include "Api/Assets/Models/Asset.h"
#include "Cache/AssetStorageManager.h"
#include "Interfaces/IHttpResponse.h"

const FString FAssetSaver::CacheFolderPath = FPaths::ProjectPersistentDownloadDir() / TEXT("ReadyPlayerMe/AssetCache");


FAssetSaver::FAssetSaver(): bIsImageLoaded(false), bIsGlbLoaded(false)
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
	LoadAndSaveGlb(StoredAsset);
	LoadAndSaveImage(StoredAsset);
}

void FAssetSaver::LoadAndSaveImage(const FStoredAsset& StoredAsset)
{
	if(StoredAsset.Asset.IconUrl.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Icon URL is empty for asset: %s assetname: %s"), *StoredAsset.Asset.Id, *StoredAsset.Asset.Name);
		bIsImageLoaded = true;
		return;
	}
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(StoredAsset.Asset.IconUrl);
	Request->SetVerb(TEXT("GET"));
	// Capture TSharedPtr<FAssetSaver> in the lambda to ensure it is not destroyed prematurely
	TSharedPtr<FAssetSaver> ThisPtr = SharedThis(this);
	Request->OnProcessRequestComplete().BindLambda([ThisPtr, StoredAsset](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		ThisPtr->OnAssetLoaded(Request, Response, bWasSuccessful, &StoredAsset);
	});

	Request->ProcessRequest();
}

void FAssetSaver::LoadAndSaveGlb(const FStoredAsset& StoredAsset)
{
	if(StoredAsset.Asset.GlbUrl.IsEmpty())
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Glb URL is empty for asset: %s assetname: %s"), *StoredAsset.Asset.Id, *StoredAsset.Asset.Type);
		bIsImageLoaded = true;
		return;
	}
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(StoredAsset.Asset.GlbUrl);
	Request->SetVerb(TEXT("GET"));

	// Capture TSharedPtr<FAssetSaver> in the lambda to ensure it is not destroyed prematurely
	TSharedPtr<FAssetSaver> ThisPtr = SharedThis(this);
	Request->OnProcessRequestComplete().BindLambda([ThisPtr, StoredAsset](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		ThisPtr->OnAssetLoaded(Request, Response, bWasSuccessful, &StoredAsset);
	});

	Request->ProcessRequest();
}

void FAssetSaver::OnAssetLoaded(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful, const FStoredAsset* StoredAsset)
{
	const bool bIsGlb = Request->GetURL().EndsWith(TEXT(".glb"));
	if(bWasSuccessful && Response.IsValid())
	{
		if(bIsGlb)
		{
			SaveToFile(StoredAsset->GlbFilePath, Response->GetContent());
		}
		else
		{
			SaveToFile(StoredAsset->IconFilePath, Response->GetContent());

		}
	}
	else
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to load image or .glb from url: %s"), *Request->GetURL());
	}
	if(bIsGlb)
	{
		bIsGlbLoaded = true;
	}
	else
	{
		bIsImageLoaded = true;
	}
	if(bIsImageLoaded && bIsGlbLoaded)
	{
		FAssetStorageManager::Get().TrackStoredAsset(*StoredAsset);

		UE_LOG(LogReadyPlayerMe, Log, TEXT("Asset saved to cache"));
		OnAssetSaved.ExecuteIfBound(true);
	}
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
