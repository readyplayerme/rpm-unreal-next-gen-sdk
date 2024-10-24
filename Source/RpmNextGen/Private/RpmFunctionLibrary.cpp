// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmFunctionLibrary.h"
#include "RpmNextGen.h"
#include "Api/Assets/AssetApi.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetListResponse.h"
#include "Api/Files/PakFileUtility.h"
#include "Cache/AssetCacheManager.h"
#include "Cache/CachedAssetData.h"
#include "Settings/RpmDeveloperSettings.h"

void URpmFunctionLibrary::FetchFirstAssetId(UObject* WorldContextObject, const FString& AssetType, FOnAssetIdFetched OnAssetIdFetched)
{
    if (!WorldContextObject)
    {
        UE_LOG(LogReadyPlayerMe, Error, TEXT("WorldContextObject is null"));
        OnAssetIdFetched.ExecuteIfBound(FString());
        return;
    }

    TSharedPtr<FAssetApi> AssetApi = MakeShared<FAssetApi>();
    TSharedPtr<FOnAssetIdFetched> SharedDelegate = MakeShared<FOnAssetIdFetched>(OnAssetIdFetched);

    const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
    FAssetListQueryParams QueryParams;
    QueryParams.Type = AssetType;
    QueryParams.ApplicationId = RpmSettings->ApplicationId;
    QueryParams.Limit = 1;
    TSharedPtr<FAssetListRequest> AssetListRequest = MakeShared<FAssetListRequest>(QueryParams);

    TWeakObjectPtr<UObject> WeakContextObject(WorldContextObject);

    // TODO make this work
    // AssetApi->ListAssetsAsync(AssetListRequest, [WeakContextObject, SharedDelegate, AssetApi, AssetType](TSharedPtr<FAssetListResponse> Response, bool bWasSuccessful)
    // {
    //     if (!WeakContextObject.IsValid())
    //     {
    //         UE_LOG(LogReadyPlayerMe, Error, TEXT("WorldContextObject is no longer valid."));
    //         SharedDelegate->ExecuteIfBound(FString());
    //         return;
    //     }
    //
    //     if (bWasSuccessful && Response->Data.Num() > 0)
    //     {
    //         FString FirstAssetId = Response->Data[0].Id;
    //         UE_LOG(LogReadyPlayerMe, Warning, TEXT("FirstAssetId fetched: %s"), *FirstAssetId);
    //         SharedDelegate->ExecuteIfBound(FirstAssetId);
    //         return;
    //     }
    //
    //     // Fallback to cache if online request failed or returned no data
    //     TArray<FCachedAssetData> Assets = FAssetCacheManager::Get().GetAssetsOfType(AssetType);
    //     if (Assets.Num() > 0)
    //     {
    //         FString FirstAssetId = Assets[0].Id;
    //         UE_LOG(LogReadyPlayerMe, Warning, TEXT("FirstAssetId fetched from cache: %s"), *FirstAssetId);
    //         SharedDelegate->ExecuteIfBound(FirstAssetId);
    //         return;
    //     }
    //
    //     UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to fetch FirstAssetId"));
    //     SharedDelegate->ExecuteIfBound(FString());
    // });

    // AssetApi->OnListAssetsResponse.BindLambda([WeakContextObject, SharedDelegate, AssetApi, AssetType](const FAssetListResponse& Response, bool bWasSuccessful)
    // {
    //     if (!WeakContextObject.IsValid())
    //     {
    //         UE_LOG(LogReadyPlayerMe, Error, TEXT("WorldContextObject is no longer valid."));
    //         SharedDelegate->ExecuteIfBound(FString());
    //         return;
    //     }
    //
    //     if (bWasSuccessful && Response.Data.Num() > 0)
    //     {
    //         FString FirstAssetId = Response.Data[0].Id;
    //         UE_LOG(LogReadyPlayerMe, Warning, TEXT("FirstAssetId fetched: %s"), *FirstAssetId);
    //         SharedDelegate->ExecuteIfBound(FirstAssetId);
    //         return;
    //     }
    //
    //     // Fallback to cache if online request failed or returned no data
    //     TArray<FCachedAssetData> Assets = FAssetCacheManager::Get().GetAssetsOfType(AssetType);
    //     if (Assets.Num() > 0)
    //     {
    //         FString FirstAssetId = Assets[0].Id;
    //         UE_LOG(LogReadyPlayerMe, Warning, TEXT("FirstAssetId fetched from cache: %s"), *FirstAssetId);
    //         SharedDelegate->ExecuteIfBound(FirstAssetId);
    //         return;
    //     }
    //
    //     UE_LOG(LogReadyPlayerMe, Error, TEXT("Failed to fetch FirstAssetId"));
    //     SharedDelegate->ExecuteIfBound(FString());
    // });
    //
    // AssetApi->ListAssetsAsync(AssetListRequest);
}

void URpmFunctionLibrary::ExtractCachePakFile()
{
    const FString PakFilePath = FFileUtility::GetFullPersistentPath(FPakFileUtility::CachePakFilePath);

	FPakFileUtility::ExtractFilesFromPak(PakFilePath);
}
