#include "Cache/CacheGenerator.h"
#include "HttpModule.h"
#include "RpmNextGen.h"
#include "Api/Assets/AssetApi.h"
#include "Api/Assets/AssetLoader.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetTypeListRequest.h"
#include "Api/Auth/ApiKeyAuthStrategy.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "Settings/RpmDeveloperSettings.h"

const FString FCacheGenerator::CacheFolderPath = FPaths::ProjectPersistentDownloadDir() / TEXT("ReadyPlayerMe/AssetCache");
const FString FCacheGenerator::ZipFileName = TEXT("CacheAssets.zip");

FCacheGenerator::FCacheGenerator()
	: CurrentBaseModelIndex(0), MaxItemsPerCategory(10)
{
	Http = &FHttpModule::Get();
	AssetApi = MakeUnique<FAssetApi>();
	const URpmDeveloperSettings* Settings = GetDefault<URpmDeveloperSettings>();
	
	if(!Settings->ApiKey.IsEmpty())
	{
		AssetApi->SetAuthenticationStrategy(new FApiKeyAuthStrategy());
	}
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
	MaxItemsPerCategory = InItemsPerCategory;
	FetchBaseModels();
}

void FCacheGenerator::LoadAndStoreAssets()
{
	int RefittedAssetCount = 0;
	for (auto BaseModel : BaseModelAssets)
	{
		for (auto Pairs : BaseModelAssetsMap)
		{
			RefittedAssetCount += Pairs.Value.Num();
		}
	}	
	RequiredAssetDownloadRequest = RefittedAssetCount +  BaseModelAssets.Num();
	UE_LOG(LogReadyPlayerMe, Log, TEXT("Total assets to download: %d. Total refitted assets to fetch: %d"), RequiredAssetDownloadRequest, RequiredRefittedAssetRequests);
	for (auto BaseModel : BaseModelAssets)
	{
		const FString BaseModeFolder = CacheFolderPath / BaseModel.Id;
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		const FString DirectoryPath = FPaths::GetPath(BaseModeFolder);
		if (!PlatformFile.DirectoryExists(*DirectoryPath))
		{
			PlatformFile.CreateDirectoryTree(*DirectoryPath);
		}

		LoadAndStoreAssetFromUrl(BaseModel.Id, &BaseModel);
		for (auto Pairs : BaseModelAssetsMap)
		{
			for (auto AssetForBaseModel : Pairs.Value)
			{
				LoadAndStoreAssetFromUrl(BaseModel.Id, &AssetForBaseModel);
			}
		}
	}	
}

void FCacheGenerator::LoadAndStoreAssetFromUrl(const FString& BaseModelId, const FAsset* Asset)
{
	TSharedPtr<FAssetLoader> AssetLoader = MakeShared<FAssetLoader>();
	AssetLoader->OnAssetSaved.BindRaw( this, &FCacheGenerator::OnAssetSaved);
	AssetLoader->LoadAsset(*Asset, BaseModelId, true);
}

void FCacheGenerator::OnAssetSaved(const FAssetSaveData& AssetSaveData)
{
	AssetDownloadRequestsCompleted++;
	if(AssetDownloadRequestsCompleted >= RequiredAssetDownloadRequest)
	{
		UE_LOG(LogReadyPlayerMe, Log, TEXT("OnLocalCacheGenerated Total assets to download: %d. Asset download requests completed: %d"), RequiredAssetDownloadRequest, AssetDownloadRequestsCompleted);

		OnLocalCacheGenerated.ExecuteIfBound(true);
	}
}

void FCacheGenerator::OnListAssetsResponse(const FAssetListResponse& AssetListResponse, bool bWasSuccessful)
{
	UE_LOG(LogReadyPlayerMe, Log, TEXT("OnListAssetsResponse") );
	if(bWasSuccessful && AssetListResponse.IsSuccess)
	{
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Success ") );
		if (AssetListResponse.Data.Num() > 0 && AssetListResponse.Data[0].Type == FAssetApi::BaseModelType)
		{
			BaseModelAssets.Empty();
			BaseModelAssets.Append(AssetListResponse.Data);
			UE_LOG(LogReadyPlayerMe, Log, TEXT("Fetched %d Base models"), AssetListResponse.Data.Num());
			FetchAssetTypes();
			return;
		}
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Fetched %d assets"), AssetListResponse.Data.Num());

		if(AssetListResponse.Data.Num() > 0)
		{
			FAsset BaseModelID = BaseModelAssets[CurrentBaseModelIndex];
			if (!BaseModelAssetsMap.Contains(BaseModelID.Id))
			{
				BaseModelAssetsMap.Add(BaseModelID.Id, AssetListResponse.Data);
			}
			else
			{
				BaseModelAssetsMap[BaseModelID.Id].Append(AssetListResponse.Data);
			}
		}
		RefittedAssetRequestsCompleted++;
		if(RefittedAssetRequestsCompleted >= RequiredRefittedAssetRequests)
		{
			OnCacheDataLoaded.ExecuteIfBound(true);
		}
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to fetch assets"));
	RefittedAssetRequestsCompleted++;
	if(RefittedAssetRequestsCompleted >= RequiredRefittedAssetRequests)
	{
		OnCacheDataLoaded.ExecuteIfBound(true);
	}
}

void FCacheGenerator::FetchAssetsForEachBaseModel()
{
	RequiredRefittedAssetRequests = AssetTypes.Num() * BaseModelAssets.Num();
	for (FAsset& BaseModel : BaseModelAssets)
	{
		for(FString AssetType : AssetTypes)
		{
			FetchAssetsForBaseModel(BaseModel.Id, AssetType);
		}
	}
}

void FCacheGenerator::OnListAssetTypesResponse(const FAssetTypeListResponse& AssetListResponse, bool bWasSuccessful)
{
	if(bWasSuccessful && AssetListResponse.IsSuccess)
	{
		UE_LOG(LogReadyPlayerMe, Log, TEXT("Fetched %d asset types"), AssetListResponse.Data.Num());
		AssetTypes.Append(AssetListResponse.Data);
		FetchAssetsForEachBaseModel();
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
		const FString SavePath = CacheFolderPath / TEXT("/") / ZipFileName;

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
	// TODO add implementation
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
	QueryParams.ExcludeTypes = "baseModel";
	AssetListRequest.Params = QueryParams;
	AssetApi->ListAssetTypesAsync(AssetListRequest);
}

void FCacheGenerator::FetchAssetsForBaseModel(const FString& BaseModelID, const FString& AssetType) const
{
	URpmDeveloperSettings *Settings = GetMutableDefault<URpmDeveloperSettings>();
	FAssetListQueryParams QueryParams = FAssetListQueryParams();
	QueryParams.Type = AssetType;
	QueryParams.ApplicationId = Settings->ApplicationId;
	QueryParams.CharacterModelAssetId = BaseModelID;
	QueryParams.Limit = MaxItemsPerCategory;
	FAssetListRequest AssetListRequest = FAssetListRequest(QueryParams);
	AssetApi->ListAssetsAsync(AssetListRequest);
}
