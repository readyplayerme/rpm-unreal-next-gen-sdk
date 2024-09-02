// Fill out your copyright notice in the Description page of Project Settings.


#include "RpmFunctionLibrary.h"
#include "Api/Assets/AssetApi.h"
#include "Api/Assets/Models/AssetListRequest.h"
#include "Api/Assets/Models/AssetListResponse.h"
#include "Api/Auth/ApiKeyAuthStrategy.h"
#include "Settings/RpmDeveloperSettings.h"

void URpmFunctionLibrary::FetchFirstAssetId(UObject* WorldContextObject, const FString& AssetType, FOnAssetIdFetched OnAssetIdFetched)
{
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
		UE_LOG(LogTemp, Error, TEXT("WorldContextObject is null"));
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
