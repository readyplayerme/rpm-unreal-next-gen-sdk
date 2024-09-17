// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmFunctionLibrary.h"
#include "RpmNextGen.h"
#include "Api/Assets/AssetApi.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetListResponse.h"
#include "Api/Auth/ApiKeyAuthStrategy.h"
#include "Cache/AssetCacheManager.h"
#include "Cache/CachedAssetData.h"
#include "Settings/RpmDeveloperSettings.h"
#include "Utilities/ConnectionManager.h"

void URpmFunctionLibrary::FetchFirstAssetId(UObject* WorldContextObject, const FString& AssetType, FOnAssetIdFetched OnAssetIdFetched)
{
	if(!IsInternetConnected())
	{		
		TArray<FCachedAssetData> CachedAssets = FAssetCacheManager::Get().GetAssetsOfType(AssetType);
		if( CachedAssets.Num() > 0)
		{
			OnAssetIdFetched.ExecuteIfBound(CachedAssets[0].Id);
			return;
		}
		UE_LOG(LogReadyPlayerMe, Warning, TEXT("Unable to fetch first asset from cache."));
		return;
	}

	TSharedPtr<FAssetApi> AssetApi = MakeShared<FAssetApi>();
	const URpmDeveloperSettings* RpmSettings = GetDefault<URpmDeveloperSettings>();
	if(!RpmSettings->ApiKey.IsEmpty() || RpmSettings->ApiProxyUrl.IsEmpty())
	{
		AssetApi->SetAuthenticationStrategy(new FApiKeyAuthStrategy());
	}
	
	FAssetListQueryParams QueryParams;
	QueryParams.Type = AssetType;
	QueryParams.ApplicationId = RpmSettings->ApplicationId;
	FAssetListRequest AssetListRequest = FAssetListRequest(QueryParams);

	if (!WorldContextObject)
	{
		UE_LOG(LogReadyPlayerMe, Error, TEXT("WorldContextObject is null"));
		return;
	}

	AssetApi->OnListAssetsResponse.BindLambda([OnAssetIdFetched, AssetApi](const FAssetListResponse& Response, bool bWasSuccessful)
	{
		FString FirstAssetId;
		if (bWasSuccessful && Response.Data.Num() > 0)
		{
			FirstAssetId = Response.Data[0].Id;
		}
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
