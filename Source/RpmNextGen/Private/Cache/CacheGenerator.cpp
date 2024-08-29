#include "Cache/CacheGenerator.h"
#include "HttpModule.h"
#include "RpmNextGen.h"
#include "Api/Assets/AssetApi.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetTypeListRequest.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "Settings/RpmDeveloperSettings.h"

const FString FCacheGenerator::CacheFolderPath = FPaths::ProjectContentDir() / TEXT("ReadyPlayerMe/LocalCache");
const FString FCacheGenerator::ZipFileName = TEXT("LocalCacheAssets.zip");

FCacheGenerator::FCacheGenerator()
	: CurrentBaseModelIndex(0), ItemsPerCategory(10)
{
	Http = &FHttpModule::Get();
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
	ItemsPerCategory = InItemsPerCategory;
	FetchBaseModels();
}

void FCacheGenerator::ProcessNextRequest()
{
	
}

void FCacheGenerator::OnListAssetsResponse(const FAssetListResponse& AssetListResponse, bool bWasSuccessful)
{
	if(bWasSuccessful && AssetListResponse.IsSuccess)
	{
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
		
		// // Check if more asset types need to be requested for the current base model
		// CurrentBaseModelIndex++;
		// if (CurrentBaseModelIndex < BaseModelAssets.Num())
		// {
		// 	FetchAssetsForBaseModel(BaseModelAssets[CurrentBaseModelIndex].Id);
		// }
		// else
		// {
		// 	UE_LOG(LogTemp, Log, TEXT("All assets have been fetched successfully."));
		// 	// Final processing or callback can go here
		// }
		return;
	}
	UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to fetch assets"));
	OnGenerateLocalCacheDelegate.ExecuteIfBound(false);
	
}

void FCacheGenerator::FetchAssetsForEachBaseModel()
{
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
	OnGenerateLocalCacheDelegate.ExecuteIfBound(false);
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

void FCacheGenerator::OnRequestCacheAssetsComplete(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		OnGenerateLocalCacheDelegate.ExecuteIfBound(true);
		return;
	}
	OnGenerateLocalCacheDelegate.ExecuteIfBound(false);
}

void FCacheGenerator::ExtractCache()
{
	
}

void FCacheGenerator::FetchBaseModels()
{
	URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>();
	FAssetListRequest AssetListRequest = FAssetListRequest();
	FAssetListQueryParams QueryParams = FAssetListQueryParams();
	QueryParams.ApplicationId = Settings->ApplicationId;
	QueryParams.Type = FAssetApi::BaseModelType;
	AssetListRequest.Params = QueryParams;
	AssetApi->ListAssetsAsync(AssetListRequest);
}

void FCacheGenerator::FetchAssetTypes()
{
	URpmDeveloperSettings* Settings = GetMutableDefault<URpmDeveloperSettings>();
	FAssetTypeListRequest AssetListRequest;
	FAssetTypeListQueryParams QueryParams = FAssetTypeListQueryParams();
	QueryParams.ApplicationId = Settings->ApplicationId;
	AssetListRequest.Params = QueryParams;
	AssetApi->ListAssetTypesAsync(AssetListRequest);
}

void FCacheGenerator::FetchAssetsForBaseModel(const FString& BaseModelID, const FString& AssetType)
{
	URpmDeveloperSettings *Settings = GetMutableDefault<URpmDeveloperSettings>();
	FAssetListQueryParams QueryParams = FAssetListQueryParams();
	QueryParams.Type = AssetType;
	QueryParams.ApplicationId = Settings->ApplicationId;
	QueryParams.CharacterModelAssetId = BaseModelID;
	QueryParams.Limit = ItemsPerCategory;
	FAssetListRequest AssetListRequest = FAssetListRequest(QueryParams);
	AssetApi->ListAssetsAsync(AssetListRequest);
}
