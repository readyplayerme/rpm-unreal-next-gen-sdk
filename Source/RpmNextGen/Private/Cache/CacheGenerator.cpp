#include "Cache/CacheGenerator.h"
#include "HttpModule.h"
#include "RpmNextGen.h"
#include "Api/Assets/AssetApi.h"
#include "Api/Assets/AssetGlbLoader.h"
#include "Api/Assets/AssetIconLoader.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetTypeListRequest.h"
#include "Api/Files/PakFileUtility.h"
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
	AssetApi = MakeUnique<FAssetApi>();
	AssetApi->OnListAssetsResponse.BindRaw(this, &FCacheGenerator::OnListAssetsResponse);
	AssetApi->OnListAssetTypeResponse.BindRaw(this, &FCacheGenerator::OnListAssetTypesResponse);
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
	int AssetIconRequestCount = 0;
	
	// load and store base model assets
	for ( auto Asset : BaseModelAssets)
	{
		LoadAndStoreAssetIcon(Asset.Id, &Asset);
		AssetIconRequestCount++;
	}
	
	// load and store asset icon (only 1 set of icons is required)
	for (auto Asset : AssetMapByBaseModelId[BaseModelAssets[0].Id])
	{
		if(Asset.Type == FAssetApi::BaseModelType) continue;			
		LoadAndStoreAssetIcon(BaseModelAssets[0].Id, &Asset);
		AssetIconRequestCount++;
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
	TSharedPtr<FAssetGlbLoader> AssetLoader = MakeShared<FAssetGlbLoader>();
	AssetLoader->OnGlbLoaded.BindRaw( this, &FCacheGenerator::OnAssetGlbSaved);
	AssetLoader->LoadGlb(*Asset, BaseModelId, true);
}

void FCacheGenerator::LoadAndStoreAssetIcon(const FString& BaseModelId, const FAsset* Asset)
{
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
	}
}

void FCacheGenerator::OnAssetIconSaved(const FAsset& Asset, const TArray<uint8>& Data)
{
	NumberOfAssetsSaved++;
	if(NumberOfAssetsSaved >= RequiredAssetDownloadRequest)
	{
		UE_LOG(LogReadyPlayerMe, Log, TEXT("OnLocalCacheGenerated Total assets to download: %d. Asset download requests completed: %d"), RequiredAssetDownloadRequest, NumberOfAssetsSaved);

		OnLocalCacheGenerated.ExecuteIfBound(true);
	}
}

void FCacheGenerator::OnListAssetsResponse(const FAssetListResponse& AssetListResponse, bool bWasSuccessful)
{
	if(bWasSuccessful && AssetListResponse.IsSuccess)
	{
		if (AssetListResponse.Data[0].Type == FAssetApi::BaseModelType)
		{
			for ( auto BaseModel : AssetListResponse.Data)
			{
				TArray<FAsset> AssetList = TArray<FAsset>();
				AssetList.Add(BaseModel);
				AssetMapByBaseModelId.Add(BaseModel.Id, AssetList);
			}
			UE_LOG(LogReadyPlayerMe, Log, TEXT("Fetched %d base models"), AssetListResponse.Data.Num());
			FetchAssetTypes();
			return;
		}
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Fetched %d assets of type %s"), AssetListResponse.Data.Num(), *AssetListResponse.Data[0].Type);
		
		if(AssetListResponse.Data.Num() > 0)
		{
			FString BaseModelID = AssetListRequests[RefittedAssetRequestsCompleted].Params.CharacterModelAssetId;
			if (!AssetMapByBaseModelId.Contains(BaseModelID))
			{
				AssetMapByBaseModelId.Add(BaseModelID, AssetListResponse.Data);
			}
			else
			{
				AssetMapByBaseModelId[BaseModelID].Append(AssetListResponse.Data);
			}

		}
		RefittedAssetRequestsCompleted++;
		FetchNextRefittedAsset();
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to fetch assets"));
	RefittedAssetRequestsCompleted++;
	FetchNextRefittedAsset();
}

void FCacheGenerator::StartFetchingRefittedAssets()
{
	RefittedAssetRequestsCompleted = 0;

	URpmDeveloperSettings *Settings = GetMutableDefault<URpmDeveloperSettings>();
	AssetListRequests = TArray<FAssetListRequest>();
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
			FAssetListRequest AssetListRequest = FAssetListRequest(QueryParams);
			AssetListRequests.Add(AssetListRequest);
		}
	}
	FetchNextRefittedAsset();
}

void FCacheGenerator::FetchNextRefittedAsset()
{
	if(RefittedAssetRequestsCompleted >= AssetListRequests.Num())
	{
		OnCacheDataLoaded.ExecuteIfBound(true);
		return;
	}
	AssetApi->ListAssetsAsync(AssetListRequests[RefittedAssetRequestsCompleted]);
}

void FCacheGenerator::OnListAssetTypesResponse(const FAssetTypeListResponse& AssetListResponse, bool bWasSuccessful)
{
	if(bWasSuccessful && AssetListResponse.IsSuccess)
	{
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Fetched %d asset types"), AssetListResponse.Data.Num());
		AssetTypes.Append(AssetListResponse.Data);
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

void FCacheGenerator::FetchBaseModels() const
{
	URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>();
	FAssetListRequest AssetListRequest = FAssetListRequest();
	FAssetListQueryParams QueryParams = FAssetListQueryParams();
	QueryParams.ApplicationId = Settings->ApplicationId;
	QueryParams.Type = FAssetApi::BaseModelType;
	AssetListRequest.Params = QueryParams;
	AssetApi->ListAssetsAsync(AssetListRequest);
	UE_LOG(LogReadyPlayerMe, Log, TEXT("Fetching base models") );
}

void FCacheGenerator::FetchAssetTypes() const
{
	URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>();
	FAssetTypeListRequest AssetListRequest;
	FAssetTypeListQueryParams QueryParams = FAssetTypeListQueryParams();
	QueryParams.ApplicationId = Settings->ApplicationId;
	AssetListRequest.Params = QueryParams;
	AssetApi->ListAssetTypesAsync(AssetListRequest);
}
