#include "Cache/CacheGenerator.h"
#include "HttpModule.h"
#include "RpmNextGen.h"
#include "Api/Assets/AssetApi.h"
#include "Api/Assets/AssetGlbLoader.h"
#include "Api/Assets/AssetIconLoader.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetTypeListRequest.h"
#include "Cache/AssetCacheManager.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "Settings/RpmDeveloperSettings.h"

const FString FCacheGenerator::ZipFileName = TEXT("CacheAssets.pak");

FCacheGenerator::FCacheGenerator() : CurrentBaseModelIndex(0), MaxItemsPerCategory(10)
{
	Http = &FHttpModule::Get();
	AssetApi = MakeUnique<FAssetApi>(EApiRequestStrategy::ApiOnly);
}

FCacheGenerator::~FCacheGenerator()
{
}

void FCacheGenerator::DownloadRemoteCacheFromUrl(const FString& Url)
{
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));
	Request->OnProcessRequestComplete().BindRaw(this, &FCacheGenerator::OnDownloadRemoteCacheComplete);
	Request->ProcessRequest();
}

void FCacheGenerator::GenerateLocalCache(int InItemsPerCategory)
{
	Reset();
	MaxItemsPerCategory = InItemsPerCategory;
	FetchBaseModels();
}

void FCacheGenerator::LoadAndStoreAssets()
{
	TArray<FAsset> BaseModelAssets = TArray<FAsset>();
	int TotalRefittedAssets = 0;
	
	// Ensure AssetMapByBaseModelId contains valid data
	if (AssetMapByBaseModelId.Num() == 0)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("No base models found in AssetMapByBaseModelId"));
		return;
	}
	for ( auto BaseModel : AssetMapByBaseModelId)
	{
		for (auto Asset : BaseModel.Value)
		{
			if(Asset.Type == FAssetApi::BaseModelType)
			{
				BaseModelAssets.Add(Asset);
			}
			TotalRefittedAssets++;
		}
	}

	// Ensure there's at least one BaseModel
	if (BaseModelAssets.Num() == 0)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("No base model assets found"));
		OnCacheDataLoaded.ExecuteIfBound(false);		
		return;
	}
	
	int AssetIconRequestCount = 0;
	
	// load and store base model assets
	for ( auto Asset : BaseModelAssets)
	{
		LoadAndStoreAssetIcon(Asset.Id, &Asset);
		AssetIconRequestCount++;
	}
	
	// Ensure AssetMap contains the BaseModelId
	if (AssetMapByBaseModelId.Contains(BaseModelAssets[0].Id))
	{
		for (auto& Asset : AssetMapByBaseModelId[BaseModelAssets[0].Id])
		{
			if (Asset.Type == FAssetApi::BaseModelType) continue;
			LoadAndStoreAssetIcon(BaseModelAssets[0].Id, &Asset);
			AssetIconRequestCount++;
		}
	}
	else
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("BaseModelId not found in AssetMapByBaseModelId"));
		return;
	}
	
	 RequiredAssetDownloadRequest = TotalRefittedAssets + AssetIconRequestCount;
	 const FString GlobalCachePath = FFileUtility::GetCachePath();
	 UE_LOG(LogReadyPlayerMe, Log, TEXT("Total assets to download: %d. Total refitted assets glbs to fetch: %d"), RequiredAssetDownloadRequest, TotalRefittedAssets - BaseModelAssets.Num());

	for (auto Pair : AssetMapByBaseModelId)
	{
		const FString BaseModeFolder = GlobalCachePath / Pair.Key;
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		const FString DirectoryPath = FPaths::GetPath(BaseModeFolder);
		if (!PlatformFile.DirectoryExists(*DirectoryPath))
		{
			PlatformFile.CreateDirectoryTree(*DirectoryPath);
		}
		for (auto Asset : Pair.Value)
		{
			LoadAndStoreAssetGlb(Pair.Key, &Asset);
		}
	}
}

void FCacheGenerator::LoadAndStoreAssetGlb(const FString& BaseModelId, const FAsset* Asset)
{
	if (!Asset) // Ensure asset is valid
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Invalid asset when loading GLB for BaseModelId: %s"), *BaseModelId);
		return;
	}

	TSharedPtr<FAssetGlbLoader> AssetLoader = MakeShared<FAssetGlbLoader>();
	AssetLoader->OnGlbLoaded.BindRaw(this, &FCacheGenerator::OnAssetGlbSaved);
	AssetLoader->LoadGlb(*Asset, BaseModelId, true);
}

void FCacheGenerator::LoadAndStoreAssetIcon(const FString& BaseModelId, const FAsset* Asset)
{
	if (!Asset) // Ensure asset is valid
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("Invalid asset when loading Icon for BaseModelId: %s"), *BaseModelId);
		return;
	}
	TSharedPtr<FAssetIconLoader> AssetLoader = MakeShared<FAssetIconLoader>();
	AssetLoader->OnIconLoaded.BindRaw( this, &FCacheGenerator::OnAssetIconSaved);
	AssetLoader->LoadIcon(*Asset, true);
}

void FCacheGenerator::Reset()
{
	AssetMapByBaseModelId.Empty();
	AssetListRequests.Empty();
	AssetTypes.Empty();
	CurrentBaseModelIndex = 0;
	RefittedAssetRequestsCompleted = 0;
	RequiredAssetDownloadRequest = 0;
	NumberOfAssetsSaved = 0;
	FAssetCacheManager::Get().ClearAllCache();
}

void FCacheGenerator::OnAssetGlbSaved(const FAsset& Asset, const TArray<uint8>& Data)
{
	NumberOfAssetsSaved++;
	if(NumberOfAssetsSaved >= RequiredAssetDownloadRequest)
	{
		UE_LOG(LogReadyPlayerMe, Log, TEXT("OnLocalCacheGenerated Total assets to download: %d. Asset download requests completed: %d"), RequiredAssetDownloadRequest, NumberOfAssetsSaved);

		OnLocalCacheGenerated.ExecuteIfBound(true);
		AddFolderToNonAssetDirectory();
	}
}

void FCacheGenerator::OnAssetIconSaved(const FAsset& Asset, const TArray<uint8>& Data)
{
	NumberOfAssetsSaved++;
	if(NumberOfAssetsSaved >= RequiredAssetDownloadRequest)
	{
		UE_LOG(LogReadyPlayerMe, Log, TEXT("OnLocalCacheGenerated Total assets to download: %d. Asset download requests completed: %d"), RequiredAssetDownloadRequest, NumberOfAssetsSaved);

		OnLocalCacheGenerated.ExecuteIfBound(true);
		AddFolderToNonAssetDirectory();
	}
}

void FCacheGenerator::AddFolderToNonAssetDirectory() const
{
	FString ConfigFilePath = FPaths::ProjectConfigDir() / TEXT("DefaultGame.ini");

	// Section and key for "Additional Non-Asset Directories to Copy"
	const FString SectionName = TEXT("/Script/UnrealEd.ProjectPackagingSettings");
	const FString KeyName = TEXT("+DirectoriesToAlwaysStageAsNonUFS");

	// Folder to add to the non-asset directories
	const FString FolderToAdd = FString::Printf(TEXT("(Path=\"%s\")"), *FFileUtility::RelativeCachePath);

	// Check if the folder is already added
	FString CurrentValue;
	if (GConfig->GetString(*SectionName, *KeyName, CurrentValue, ConfigFilePath))
	{
		if (CurrentValue.Contains(FolderToAdd))
		{
			// Folder already exists, no need to add it
			UE_LOG(LogReadyPlayerMe, Log, TEXT("Folder already added to Additional Non-Asset Directories: %s"), *FolderToAdd);
			return;
		}
	}

	// Add the folder to the config
	GConfig->SetString(*SectionName, *KeyName, *FolderToAdd, ConfigFilePath);

	// Force update the config file
	GConfig->Flush(false, ConfigFilePath);

	UE_LOG(LogReadyPlayerMe, Log, TEXT("Added folder to Additional Non-Asset Directories: %s"), *FolderToAdd);
}

// void FCacheGenerator::OnListAssetsResponse(const FAssetListResponse& AssetListResponse, bool bWasSuccessful)
// {
// 	if(bWasSuccessful && AssetListResponse.IsSuccess && AssetListResponse.Data.Num() > 0)
// 	{
// 		if (AssetListResponse.Data[0].Type == FAssetApi::BaseModelType)
// 		{
// 			for ( auto BaseModel : AssetListResponse.Data)
// 			{
// 				TArray<FAsset> AssetList = TArray<FAsset>();
// 				AssetList.Add(BaseModel);
// 				AssetMapByBaseModelId.Add(BaseModel.Id, AssetList);
// 			}
// 			UE_LOG(LogReadyPlayerMe, Log, TEXT("Fetched %d base models"), AssetListResponse.Data.Num());
// 			FetchAssetTypes();
// 			return;
// 		}
// 		UE_LOG(LogReadyPlayerMe, Log, TEXT("Fetched %d assets of type %s"), AssetListResponse.Data.Num(), *AssetListResponse.Data[0].Type);
// 		
// 		if(AssetListResponse.Data.Num() > 0)
// 		{
// 			FString BaseModelID = AssetListRequests[RefittedAssetRequestsCompleted].Params.CharacterModelAssetId;
// 			if (!AssetMapByBaseModelId.Contains(BaseModelID))
// 			{
// 				AssetMapByBaseModelId.Add(BaseModelID, AssetListResponse.Data);
// 			}
// 			else
// 			{
// 				AssetMapByBaseModelId[BaseModelID].Append(AssetListResponse.Data);
// 			}
//
// 		}
// 		RefittedAssetRequestsCompleted++;
// 		FetchNextRefittedAsset();
// 		return;
// 	}
// 	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to fetch assets"));
// 	OnCacheDataLoaded.ExecuteIfBound(false);
// }

void FCacheGenerator::OnListBaseModelsComplete(TSharedPtr<FAssetListResponse> AssetListResponse, bool bWasSuccessful)
{
	if(bWasSuccessful && AssetListResponse->IsSuccess && AssetListResponse->Data.Num() > 0)
	{
		for ( auto BaseModel : AssetListResponse->Data)
		{
			TArray<FAsset> AssetList = TArray<FAsset>();
			AssetList.Add(BaseModel);
			AssetMapByBaseModelId.Add(BaseModel.Id, AssetList);
		}
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Fetched %d base models"), AssetListResponse->Data.Num());
		FetchAssetTypes();
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to fetch base models"));
	OnCacheDataLoaded.ExecuteIfBound(false);	
}

void FCacheGenerator::OnListAssetsComplete(TSharedPtr<FAssetListResponse> AssetListResponse, bool bWasSuccessful)
{
	if(bWasSuccessful && AssetListResponse->IsSuccess && AssetListResponse->Data.Num() > 0)
	{
		if (AssetListResponse->Data[0].Type == FAssetApi::BaseModelType)
		{
			UE_LOG(LogReadyPlayerMe, Log, TEXT("Unexpected asset type %s. Skipping"), *FAssetApi::BaseModelType);
			return;
		}
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Fetched %d assets of type %s"), AssetListResponse->Data.Num(), *AssetListResponse->Data[0].Type);
		
		FString BaseModelID = AssetListRequests[RefittedAssetRequestsCompleted]->Params.CharacterModelAssetId;
		if (!AssetMapByBaseModelId.Contains(BaseModelID))
		{
			AssetMapByBaseModelId.Add(BaseModelID, AssetListResponse->Data);
		}
		else
		{
			AssetMapByBaseModelId[BaseModelID].Append(AssetListResponse->Data);
		}
		RefittedAssetRequestsCompleted++;
		FetchNextRefittedAsset();
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to fetch assets"));
	OnCacheDataLoaded.ExecuteIfBound(false);
}

void FCacheGenerator::StartFetchingRefittedAssets()
{
	RefittedAssetRequestsCompleted = 0;

	URpmDeveloperSettings *Settings = GetMutableDefault<URpmDeveloperSettings>();
	AssetListRequests = TArray<TSharedPtr<FAssetListRequest>>();
	for ( auto BaseModel : AssetMapByBaseModelId)
	{
		for(FString AssetType : AssetTypes)
		{
			if(AssetType == FAssetApi::BaseModelType)
			{
				continue;
			}
			FAssetListQueryParams QueryParams = FAssetListQueryParams();
			QueryParams.Type = AssetType;
			QueryParams.ApplicationId = Settings->ApplicationId;
			QueryParams.CharacterModelAssetId = BaseModel.Key;
			QueryParams.Limit = MaxItemsPerCategory;
			TSharedPtr<FAssetListRequest> AssetListRequest = MakeShared<FAssetListRequest>(QueryParams);
			AssetListRequests.Add(AssetListRequest);
		}
	}
	FetchNextRefittedAsset();
}

void FCacheGenerator::FetchBaseModels()
{
	const URpmDeveloperSettings* Settings = GetDefault<URpmDeveloperSettings>();
	TSharedPtr<FAssetListRequest> AssetListRequest = MakeShared<FAssetListRequest>();
	FAssetListQueryParams QueryParams = FAssetListQueryParams();
	QueryParams.ApplicationId = Settings->ApplicationId;
	QueryParams.Type = FAssetApi::BaseModelType;
	AssetListRequest->Params = QueryParams;

	TWeakPtr<FCacheGenerator> WeakPtrThis = AsShared();

	AssetApi->ListAssetsAsync(AssetListRequest, FOnListAssetsResponse::CreateLambda([WeakPtrThis](TSharedPtr<FAssetListResponse> Response, bool bWasSuccessful)
	{
		 if(WeakPtrThis.IsValid())
		 {
			 WeakPtrThis.Pin()->OnListBaseModelsComplete(Response, bWasSuccessful);
		 }
	}));

	UE_LOG(LogReadyPlayerMe, Log, TEXT("Fetching base models") );
}

void FCacheGenerator::FetchAssetTypes()
{
	const URpmDeveloperSettings* Settings = GetDefault<URpmDeveloperSettings>();
	TSharedPtr<FAssetTypeListRequest> AssetListRequest = MakeShared<FAssetTypeListRequest>();
	FAssetTypeListQueryParams QueryParams = FAssetTypeListQueryParams();
	QueryParams.ApplicationId = Settings->ApplicationId;
	AssetListRequest->Params = QueryParams;	
	TWeakPtr<FCacheGenerator> WeakPtrThis = AsShared();

	AssetApi->ListAssetTypesAsync(AssetListRequest, FOnListAssetTypesResponse::CreateLambda([WeakPtrThis](TSharedPtr<FAssetTypeListResponse> Response, bool bWasSuccessful)
	{
		if(WeakPtrThis.IsValid())
		{
			WeakPtrThis.Pin()->OnListAssetTypesComplete(Response, bWasSuccessful);
		}
	}));
}

void FCacheGenerator::FetchNextRefittedAsset()
{
	if(RefittedAssetRequestsCompleted >= AssetListRequests.Num())
	{
		OnCacheDataLoaded.ExecuteIfBound(true);
		return;
	}
	TWeakPtr<FCacheGenerator> WeakPtrThis = AsShared();
	AssetApi->ListAssetsAsync(AssetListRequests[RefittedAssetRequestsCompleted], FOnListAssetsResponse::CreateLambda([WeakPtrThis](TSharedPtr<FAssetListResponse> Response, bool bWasSuccessful)
	{
		if(WeakPtrThis.IsValid())
		{
			WeakPtrThis.Pin()->OnListAssetsComplete(Response, bWasSuccessful);
		}
	}));
}

// void FCacheGenerator::OnListAssetTypesResponse(const FAssetTypeListResponse& AssetListResponse, bool bWasSuccessful)
// {
// 	if(bWasSuccessful && AssetListResponse.IsSuccess)
// 	{
// 		UE_LOG(LogReadyPlayerMe, Log, TEXT("Fetched %d asset types"), AssetListResponse.Data.Num());
// 		AssetTypes.Append(AssetListResponse.Data);
// 		FAssetCacheManager::Get().StoreAssetTypes(AssetTypes);
// 		StartFetchingRefittedAssets();
// 		return;
// 	}
// 	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to fetch asset types"));
// 	OnCacheDataLoaded.ExecuteIfBound(false);
// }

void FCacheGenerator::OnListAssetTypesComplete(TSharedPtr<FAssetTypeListResponse> AssetTypeListResponse, bool bWasSuccessful)
{
	if(bWasSuccessful && AssetTypeListResponse->IsSuccess)
	{
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Fetched %d asset types"), AssetTypeListResponse->Data.Num());
		AssetTypes.Append(AssetTypeListResponse->Data);
		FAssetCacheManager::Get().StoreAssetTypes(AssetTypes);
		StartFetchingRefittedAssets();
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to fetch asset types"));
	OnCacheDataLoaded.ExecuteIfBound(false);
}

void FCacheGenerator::OnDownloadRemoteCacheComplete(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		// Get the response data
		const TArray<uint8>& Data = Response->GetContent();
		// Define the path to save the ZIP file
		const FString SavePath = FFileUtility::GetCachePath()/ TEXT("/") / ZipFileName;

		// Ensure the directory exists
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		const FString DirectoryPath = FPaths::GetPath(SavePath);
		if (!PlatformFile.DirectoryExists(*DirectoryPath))
		{
			PlatformFile.CreateDirectoryTree(*DirectoryPath);
		}

		// Save the data as a .zip file
		if (FFileHelper::SaveArrayToFile(Data, *SavePath))
		{
			UE_LOG(LogReadyPlayerMe, Log, TEXT("Successfully saved the remote cache to: %s"), *SavePath);
			OnDownloadRemoteCacheDelegate.ExecuteIfBound(true);
		}
		else
		{
			UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to save the remote cache to: %s"), *SavePath);
			OnDownloadRemoteCacheDelegate.ExecuteIfBound(false);
		}
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to download the remote cache"));
	OnDownloadRemoteCacheDelegate.ExecuteIfBound(false);
}

void FCacheGenerator::ExtractCache()
{
	//FPakFileUtility::ExtractFilesFromPak( FRpmNextGenModule::GetGlobalAssetCachePath() / TEXT("/") / ZipFileName);
}
