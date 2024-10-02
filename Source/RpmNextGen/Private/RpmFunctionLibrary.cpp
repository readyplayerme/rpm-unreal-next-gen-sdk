// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmFunctionLibrary.h"
#include "RpmNextGen.h"
#include "Api/Assets/AssetApi.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetListResponse.h"
#include "Api/Auth/ApiKeyAuthStrategy.h"
#include "Api/Files/PakFileUtility.h"
#include "Cache/AssetCacheManager.h"
#include "Cache/CachedAssetData.h"
#include "Settings/RpmDeveloperSettings.h"
#include "Utilities/ConnectionManager.h"

void URpmFunctionLibrary::FetchFirstAssetId(UObject* WorldContextObject, const FString& AssetType, FOnAssetIdFetched OnAssetIdFetched)
{
    UE_LOG(LogReadyPlayerMe, Warning, TEXT("FETCH FIRST ASSET ID"));

    if (!WorldContextObject)
    {
        UE_LOG(LogReadyPlayerMe, Error, TEXT("WorldContextObject is null"));
        return;
    }

    if (!IsInternetConnected())
    {
        TArray<FCachedAssetData> CachedAssets = FAssetCacheManager::Get().GetAssetsOfType(AssetType);
        if (CachedAssets.Num() > 0)
        {
            OnAssetIdFetched.ExecuteIfBound(CachedAssets[0].Id);
            return;
        }
        UE_LOG(LogReadyPlayerMe, Warning, TEXT("Unable to fetch first asset from cache."));
        return;
    }

    // Store the AssetApi inside the WorldContextObject to bind its lifecycle
    TWeakObjectPtr<UObject> WeakContextObject(WorldContextObject);
    TSharedPtr<FAssetApi> AssetApi = MakeShared<FAssetApi>();

    const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
    FAssetListQueryParams QueryParams;
    QueryParams.Type = AssetType;
    QueryParams.ApplicationId = RpmSettings->ApplicationId;
    QueryParams.Limit = 1;
    FAssetListRequest AssetListRequest = FAssetListRequest(QueryParams);

    // Bind the lambda to the lifecycle of the WorldContextObject
    AssetApi->OnListAssetsResponse.BindLambda([WeakContextObject, OnAssetIdFetched](const FAssetListResponse& Response, bool bWasSuccessful)
    {
        if (!WeakContextObject.IsValid()) // Ensure the object is still valid
        {
            UE_LOG(LogReadyPlayerMe, Error, TEXT("WorldContextObject is no longer valid."));
            return;
        }

        FString FirstAssetId;
        if (bWasSuccessful && Response.Data.Num() > 0)
        {
            FirstAssetId = Response.Data[0].Id;
            OnAssetIdFetched.ExecuteIfBound(FirstAssetId);
            UE_LOG(LogReadyPlayerMe, Warning, TEXT("FirstAssetId is NOT empty"));
            return;
        }

        TArray<FCachedAssetData> Assets = FAssetCacheManager::Get().GetAssetsOfType("baseModel");
        if (Assets.Num() > 0)
        {
            FirstAssetId = Assets[0].Id;
            OnAssetIdFetched.ExecuteIfBound(FirstAssetId);
            UE_LOG(LogReadyPlayerMe, Warning, TEXT("FirstAssetId is NOT empty (from cache)"));
            return;
        }

        UE_LOG(LogReadyPlayerMe, Error, TEXT("FirstAssetId is empty"));
        OnAssetIdFetched.ExecuteIfBound(FirstAssetId);
    });

    AssetApi->ListAssetsAsync(AssetListRequest);
}


bool URpmFunctionLibrary::IsInternetConnected()
{
	return FConnectionManager::Get().IsConnected();
}

void URpmFunctionLibrary::CheckInternetConnection(const FOnConnectionStatusRefreshedDelegate& OnConnectionStatusRefreshed)
{
	FConnectionManager::Get().OnConnectionStatusRefreshed.BindLambda([OnConnectionStatusRefreshed](bool bIsConnected)
	{
		OnConnectionStatusRefreshed.ExecuteIfBound(bIsConnected);
	});

	FConnectionManager::Get().CheckInternetConnection();
}

void URpmFunctionLibrary::ExtractCachePakFile()
{
	FString PakFilePath = FFileUtility::GetFullPersistentPath(FPakFileUtility::CachePakFilePath);

	FPakFileUtility::ExtractFilesFromPak(PakFilePath);
}
